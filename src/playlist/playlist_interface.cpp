#include "playlist_interface.h"
#include "playlist_manager.h"
#include <QStringList>
#include <QFileInfo>

PlaylistInterface::PlaylistInterface(QObject *parent) : QObject(parent), m_manager(nullptr)
{
}

PlaylistInterface::~PlaylistInterface()
{
    if (m_manager) {
        playlist_manager_free(m_manager);
        m_manager = nullptr;
    }
}

bool PlaylistInterface::initialize(const QString &dataDirectory)
{
    if (m_manager) {
        playlist_manager_free(m_manager);
        m_manager = nullptr;
    }
    QByteArray dirUtf8 = dataDirectory.toUtf8();
    m_manager = playlist_manager_init(dirUtf8.constData());
    return m_manager != nullptr;
}

bool PlaylistInterface::addToFavorites(const QString &title, const QString &artist, const QString &album, 
                                     const QString &filePath, const QString &coverPath, const QString &lrcPath, 
                                     int duration)
{
    if (!ensureManagerInitialized()) return false;
    SongInfo *songInfo = static_cast<SongInfo *>(createSongInfo(title, artist, album, filePath, coverPath, lrcPath, duration));
    if (!songInfo) return false;
    
    bool result = add_to_favorites(m_manager, songInfo);
    if (!result) {
        free_song_info(songInfo);
    }
    
    return result;
}

QStringList PlaylistInterface::getFavoritesSongs()
{
    if (!ensureManagerInitialized())
        return QStringList();
    return getSongsFromPlaylist(get_favorites(m_manager));
}

bool PlaylistInterface::removeFromFavorites(const QString &filePath)
{
    if (!ensureManagerInitialized()) return false;
    QByteArray pathUtf8 = filePath.toUtf8();
    return remove_from_favorites(m_manager, pathUtf8.constData());
}

bool PlaylistInterface::isInFavorites(const QString &filePath)
{
    if (!ensureManagerInitialized()) return false;
    QByteArray pathUtf8 = filePath.toUtf8();
    return is_in_favorites(m_manager, pathUtf8.constData());
}

QStringList PlaylistInterface::getAllPlaylistNames()
{
    if (!ensureManagerInitialized()) return QStringList();
    
    int count = 0;
    char **names = get_all_playlist_names(m_manager, &count);
    if (!names) return QStringList();
    
    QStringList result;
    for (int i = 0; i < count; i++) {
        result.append(QString::fromUtf8(names[i]));
    }
    
    free_playlist_names(names, count);
    return result;
}

bool PlaylistInterface::createPlaylist(const QString &name)
{
    if (!ensureManagerInitialized()) return false;
    QByteArray nameUtf8 = name.toUtf8();
    return create_playlist(m_manager, nameUtf8.constData()) != nullptr;
}

bool PlaylistInterface::deletePlaylist(const QString &name)
{
    if (!ensureManagerInitialized()) return false;
    QByteArray nameUtf8 = name.toUtf8();
    return delete_playlist(m_manager, nameUtf8.constData());
}

bool PlaylistInterface::addToPlaylist(const QString &playlistName, const QString &title, const QString &artist, 
                                     const QString &album, const QString &filePath, const QString &coverPath, 
                                     const QString &lrcPath, int duration)
{
    if (!ensureManagerInitialized()) return false;
    QByteArray playlistUtf8 = playlistName.toUtf8();
    Playlist *playlist = get_playlist(m_manager, playlistUtf8.constData());
    if (!playlist) return false;
    
    SongInfo *songInfo = static_cast<SongInfo *>(createSongInfo(title, artist, album, filePath, coverPath, lrcPath, duration));
    if (!songInfo) return false;
    
    bool result = add_to_playlist(playlist, songInfo);
    if (!result) {
        free_song_info(songInfo);
    } else {
    save_playlists(m_manager);
    }
    
    return result;
}

QStringList PlaylistInterface::getPlaylistSongs(const QString &playlistName)
{
    if (!ensureManagerInitialized()) return QStringList();
    QByteArray playlistUtf8 = playlistName.toUtf8();
    Playlist *playlist = get_playlist(m_manager, playlistUtf8.constData());
    return getSongsFromPlaylist(playlist);
}

bool PlaylistInterface::removeFromPlaylist(const QString &playlistName, const QString &filePath)
{
    if (!ensureManagerInitialized()) return false;
    QByteArray playlistUtf8 = playlistName.toUtf8();
    Playlist *playlist = get_playlist(m_manager, playlistUtf8.constData());
    if (!playlist) return false;
    QByteArray pathUtf8 = filePath.toUtf8();
    bool result = remove_from_playlist(playlist, pathUtf8.constData());
    if (result) {
        save_playlists(m_manager);
    }
    
    return result;
}

bool PlaylistInterface::savePlaylists()
{
    if (!ensureManagerInitialized()) return false;
    return save_playlists(m_manager);
}

bool PlaylistInterface::addCurrentSongToFavorites(const QString &filePath, QMediaPlayer *player)
{
    if (!ensureManagerInitialized() || !player) return false;
    
    // 从播放器获取歌曲元数据
    QString title, artist, album;
    int duration = player->duration() / 1000; // 转换为秒

    // 简单实现：直接使用文件信息作为标题
    QFileInfo fileInfo(filePath);
    title = fileInfo.baseName();

    // 添加到收藏夹（封面和歌词路径暂为空）
    return addToFavorites(title, artist, album, filePath, QString(), QString(), duration);
}

// 辅助方法实现
bool PlaylistInterface::ensureManagerInitialized()
{
    return m_manager != nullptr;
}

QString PlaylistInterface::songInfoToString(void *song)
{
    SongInfo *songInfo = static_cast<SongInfo *>(song);
    if (!songInfo) return QString();
    auto toQ = [](const char *s, const char *def){ return s ? QString::fromUtf8(s) : QString::fromUtf8(def); };
    QString title = toQ(songInfo->title, "Unknown Title");
    QString artist = toQ(songInfo->artist, "Unknown Artist");
    QString album = toQ(songInfo->album, "Unknown Album");
    QString filePath = songInfo->file_path ? QString::fromUtf8(songInfo->file_path) : QString();

    // 使用 QString::arg 链式调用在多个字段时阅读性差，直接拼接更直观
    return title + '|' + artist + '|' + album + '|' + filePath + '|' + QString::number(songInfo->duration);
}

void *PlaylistInterface::createSongInfo(const QString &title, const QString &artist, const QString &album, 
                                       const QString &filePath, const QString &coverPath, const QString &lrcPath, 
                                       int duration)
{
    // 保持 UTF-8 临时数据的生命周期，避免传递临时 constData() 后被释放
    QByteArray titleUtf8 = title.toUtf8();
    QByteArray artistUtf8 = artist.toUtf8();
    QByteArray albumUtf8 = album.toUtf8();
    QByteArray fileUtf8 = filePath.toUtf8();
    QByteArray coverUtf8 = coverPath.toUtf8();
    QByteArray lrcUtf8 = lrcPath.toUtf8();

    return create_song_info(
        titleUtf8.constData(),
        artistUtf8.constData(),
        albumUtf8.constData(),
        fileUtf8.constData(),
        coverUtf8.constData(),
        lrcUtf8.constData(),
        duration
    );
}

QStringList PlaylistInterface::getSongsFromPlaylist(void *playlist)
{
    QStringList result;
    if (!playlist) return result;
    
    PlaylistItem *item = static_cast<Playlist *>(playlist)->head;
    while (item) {
        if (item->song) {
            result.append(songInfoToString(item->song));
        }
        item = item->next;
    }
    
    return result;
}