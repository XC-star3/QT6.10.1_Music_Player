#ifndef PLAYLIST_INTERFACE_H
#define PLAYLIST_INTERFACE_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QMediaPlayer>

// Forward declare C structs from playlist_manager to keep header light-weight
struct PlaylistManager;

class PlaylistInterface : public QObject {
    Q_OBJECT

public:
    explicit PlaylistInterface(QObject *parent = nullptr);
    ~PlaylistInterface();

    // 初始化和清理
    bool initialize(const QString &dataDirectory);
    
    // 收藏夹功能
    bool addToFavorites(const QString &title, const QString &artist, const QString &album,
                       const QString &filePath, const QString &coverPath, const QString &lrcPath,
                       int duration);
    QStringList getFavoritesSongs();
    bool removeFromFavorites(const QString &filePath);
    bool isInFavorites(const QString &filePath);
    
    // 必须添加缺失的方法声明以修复编译错误
    bool savePlaylists();
    bool addCurrentSongToFavorites(const QString &filePath, QMediaPlayer *player);
    
    // 歌单管理
    QStringList getAllPlaylistNames();
    bool createPlaylist(const QString &name);
    bool deletePlaylist(const QString &name);
    
    // 歌单操作
    bool addToPlaylist(const QString &playlistName, const QString &title, const QString &artist,
                      const QString &album, const QString &filePath, const QString &coverPath,
                      const QString &lrcPath, int duration);
    QStringList getPlaylistSongs(const QString &playlistName);
    bool removeFromPlaylist(const QString &playlistName, const QString &filePath);

private:
    // 辅助方法
    bool ensureManagerInitialized();
    QString songInfoToString(void *song); // 隐藏实现细节
    void *createSongInfo(const QString &title, const QString &artist, const QString &album,
                        const QString &filePath, const QString &coverPath, const QString &lrcPath,
                        int duration);
    QStringList getSongsFromPlaylist(void *playlist);

private:
    PlaylistManager *m_manager = nullptr; // 隐藏实现细节，使用具体指针提高类型安全
};

#endif // PLAYLIST_INTERFACE_H