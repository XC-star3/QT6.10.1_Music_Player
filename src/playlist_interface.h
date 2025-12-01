#ifndef PLAYLIST_INTERFACE_H
#define PLAYLIST_INTERFACE_H

#include <QString>
#include <QStringList>
#include <QMediaPlayer>
#include <QMediaMetaData>

// 包含完整的playlist_manager头文件而不是前向声明
#include "playlist_manager.h"

// Qt接口类，封装C语言实现的播放列表管理功能
class PlaylistInterface : public QObject
{
    Q_OBJECT

public:
    // 构造函数和析构函数
    explicit PlaylistInterface(QObject *parent = nullptr);
    ~PlaylistInterface();

    // 初始化和清理
    bool initialize(const QString &dataDir = "./data/playlists");
    void cleanup();

    // 收藏夹相关操作
    bool addToFavorites(const QString &title, const QString &artist, const QString &album,
                       const QString &filePath, const QString &coverPath = "",
                       const QString &lrcPath = "", int duration = 0);
    bool removeFromFavorites(const QString &filePath);
    bool isInFavorites(const QString &filePath);
    QStringList getFavoritesSongs();

    // 歌单相关操作
    bool createPlaylist(const QString &name);
    bool deletePlaylist(const QString &name);
    QStringList getAllPlaylistNames();
    bool addToPlaylist(const QString &playlistName, const QString &title, const QString &artist,
                      const QString &album, const QString &filePath, const QString &coverPath = "",
                      const QString &lrcPath = "", int duration = 0);
    bool removeFromPlaylist(const QString &playlistName, const QString &filePath);
    QStringList getPlaylistSongs(const QString &playlistName);

    // 保存和加载
    bool savePlaylists();
    bool loadPlaylists();

    // 从QMediaPlayer获取当前歌曲信息并添加到收藏夹
    bool addCurrentSongToFavorites(const QString &filePath, class QMediaPlayer *player = nullptr);

private:
    PlaylistManager *m_manager;  // C语言实现的管理器
    QString m_dataDir;           // 数据保存目录

    // 辅助方法
    void ensureDataDirectory();
};

#endif // PLAYLIST_INTERFACE_H