#include "playlist_interface.h"

// 使用extern "C"包装C语言头文件
#ifdef __cplusplus
extern "C" {
#endif
#include "playlist_manager.h"
#ifdef __cplusplus
}
#endif

#include <QDir>
#include <QStandardPaths>
#include <QMediaPlayer>
#include <QMediaMetaData>
#include <QDebug>

PlaylistInterface::PlaylistInterface(QObject *parent) : QObject(parent), m_manager(nullptr)
{
}

PlaylistInterface::~PlaylistInterface()
{
    cleanup();
}

bool PlaylistInterface::initialize(const QString &dataDir)
{
    m_dataDir = dataDir;
    
    // 确保数据目录存在
    ensureDataDirectory();
    
    // 初始化C语言管理器
    m_manager = playlist_manager_init(m_dataDir.toUtf8().constData());
    
    if (!m_manager) {
        qWarning() << "Failed to initialize playlist manager";
        return false;
    }
    
    // 加载已保存的歌单
    loadPlaylists();
    
    return true;
}

void PlaylistInterface::cleanup()
{
    if (m_manager) {
        savePlaylists();
        playlist_manager_free(m_manager);
        m_manager = nullptr;
    }
}

void PlaylistInterface::ensureDataDirectory()
{
    QDir dir(m_dataDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
}

bool PlaylistInterface::addToFavorites(const QString &title, const QString &artist, const QString &album,
                                     const QString &filePath, const QString &coverPath, const QString &lrcPath,
                                     int duration)
{
    if (!m_manager) {
        if (!initialize()) {
            return false;
        }
    }
    
    // 创建歌曲信息
    SongInfo *song = create_song_info(
        title.toUtf8().constData(),
        artist.toUtf8().constData(),
        album.toUtf8().constData(),
        filePath.toUtf8().constData(),
        coverPath.toUtf8().constData(),
        lrcPath.toUtf8().constData(),
        duration
    );
    
    if (!song) {
        return false;
    }
    
    bool result = add_to_favorites(m_manager, song);
    
    // 如果添加失败，释放歌曲信息
    if (!result) {
        free_song_info(song);
    }
    
    return result;
}

bool PlaylistInterface::removeFromFavorites(const QString &filePath)
{
    if (!m_manager) {
        return false;
    }
    
    return remove_from_favorites(m_manager, filePath.toUtf8().constData());
}

bool PlaylistInterface::isInFavorites(const QString &filePath)
{
    if (!m_manager) {
        return false;
    }
    
    return is_in_favorites(m_manager, filePath.toUtf8().constData());
}

QStringList PlaylistInterface::getFavoritesSongs()
{
    QStringList songs;
    
    if (!m_manager) {
        return songs;
    }
    
    Playlist *favorites = get_favorites(m_manager);
    if (!favorites || !favorites->head) {
        return songs;
    }
    
    // 遍历收藏夹中的所有歌曲
    struct PlaylistItem *item = favorites->head;
    while (item) {
        if (item->song && item->song->file_path) {
            QString songInfo = QString("%1|%2|%3|%4|%5")
                .arg(item->song->title ? QString::fromUtf8(item->song->title) : "")
                .arg(item->song->artist ? QString::fromUtf8(item->song->artist) : "")
                .arg(item->song->album ? QString::fromUtf8(item->song->album) : "")
                .arg(QString::fromUtf8(item->song->file_path))
                .arg(item->song->duration);
            songs.append(songInfo);
        }
        item = item->next;
    }
    
    return songs;
}

bool PlaylistInterface::createPlaylist(const QString &name)
{
    if (!m_manager) {
        if (!initialize()) {
            return false;
        }
    }
    
    Playlist *playlist = create_playlist(m_manager, name.toUtf8().constData());
    return playlist != nullptr;
}

bool PlaylistInterface::deletePlaylist(const QString &name)
{
    if (!m_manager) {
        return false;
    }
    
    return delete_playlist(m_manager, name.toUtf8().constData());
}

QStringList PlaylistInterface::getAllPlaylistNames()
{
    QStringList names;
    
    if (!m_manager) {
        return names;
    }
    
    int count = 0;
    char **c_names = get_all_playlist_names(m_manager, &count);
    
    if (c_names && count > 0) {
        for (int i = 0; i < count; i++) {
            if (c_names[i]) {
                names.append(QString::fromUtf8(c_names[i]));
            }
        }
        free_playlist_names(c_names, count);
    }
    
    return names;
}

bool PlaylistInterface::addToPlaylist(const QString &playlistName, const QString &title, const QString &artist,
                                    const QString &album, const QString &filePath, const QString &coverPath,
                                    const QString &lrcPath, int duration)
{
    if (!m_manager) {
        if (!initialize()) {
            return false;
        }
    }
    
    // 获取或创建歌单
    Playlist *playlist = get_playlist(m_manager, playlistName.toUtf8().constData());
    if (!playlist) {
        playlist = create_playlist(m_manager, playlistName.toUtf8().constData());
        if (!playlist) {
            return false;
        }
    }
    
    // 创建歌曲信息
    SongInfo *song = create_song_info(
        title.toUtf8().constData(),
        artist.toUtf8().constData(),
        album.toUtf8().constData(),
        filePath.toUtf8().constData(),
        coverPath.toUtf8().constData(),
        lrcPath.toUtf8().constData(),
        duration
    );
    
    if (!song) {
        return false;
    }
    
    bool result = add_to_playlist(playlist, song);
    
    // 如果添加失败，释放歌曲信息
    if (!result) {
        free_song_info(song);
    } else {
        // 保存歌单
        savePlaylists();
    }
    
    return result;
}

bool PlaylistInterface::removeFromPlaylist(const QString &playlistName, const QString &filePath)
{
    if (!m_manager) {
        return false;
    }
    
    Playlist *playlist = get_playlist(m_manager, playlistName.toUtf8().constData());
    if (!playlist) {
        return false;
    }
    
    bool result = remove_from_playlist(playlist, filePath.toUtf8().constData());
    
    if (result) {
        // 保存歌单
        savePlaylists();
    }
    
    return result;
}

QStringList PlaylistInterface::getPlaylistSongs(const QString &playlistName)
{
    QStringList songs;
    
    if (!m_manager) {
        return songs;
    }
    
    Playlist *playlist = get_playlist(m_manager, playlistName.toUtf8().constData());
    if (!playlist || !playlist->head) {
        return songs;
    }
    
    // 遍历歌单中的所有歌曲
    struct PlaylistItem *item = playlist->head;
    while (item) {
        if (item->song && item->song->file_path) {
            QString songInfo = QString("%1|%2|%3|%4|%5")
                .arg(item->song->title ? QString::fromUtf8(item->song->title) : "")
                .arg(item->song->artist ? QString::fromUtf8(item->song->artist) : "")
                .arg(item->song->album ? QString::fromUtf8(item->song->album) : "")
                .arg(QString::fromUtf8(item->song->file_path))
                .arg(item->song->duration);
            songs.append(songInfo);
        }
        item = item->next;
    }
    
    return songs;
}

bool PlaylistInterface::savePlaylists()
{
    if (!m_manager) {
        return false;
    }
    
    return save_playlists(m_manager);
}

bool PlaylistInterface::loadPlaylists()
{
    if (!m_manager) {
        return false;
    }
    
    return load_playlists(m_manager);
}

bool PlaylistInterface::addCurrentSongToFavorites(const QString &filePath, QMediaPlayer *player)
{
    QString title, artist, album;
    int duration = 0;
    
    // 简化实现，暂时跳过元数据获取
    // 在实际使用时，可以根据具体Qt版本的API调整元数据获取方式
    if (player) {
        duration = player->duration() / 1000;  // 只获取时长
    }
    
    // 使用文件名作为标题
    QFileInfo fileInfo(filePath);
    title = fileInfo.baseName();
    
    // 添加到收藏夹
    return addToFavorites(title, artist, album, filePath, "", "", duration);
}