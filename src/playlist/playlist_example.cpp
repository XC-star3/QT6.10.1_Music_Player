// 收藏夹和永久歌单功能使用示例
#include "playlist_example.h"
#include "playlist_interface.h"
#include <QApplication>
#include <QDebug>
#include <QMessageBox>
#include <QInputDialog>

*/

// 简单的测试函数，展示核心功能
void testPlaylistFunctions()
{
    // 创建播放列表接口
    PlaylistInterface playlist;
    playlist.initialize("./data/playlists");
    
    // 测试添加歌曲到收藏夹
    bool result = playlist.addToFavorites("奇妙能力歌", "陈粒", "如也", "./resources/sound/奇妙能力歌.mp3", "./resources/pic/八度空间.jpg", "./resources/sound/奇妙能力歌.lrc", 258);
    qDebug() << "Add to favorites:" << result;
    
    // 测试检查是否在收藏夹中
    bool inFavorites = playlist.isInFavorites("./resources/sound/奇妙能力歌.mp3");
    qDebug() << "Is in favorites:" << inFavorites;
    
    // 测试创建歌单
    result = playlist.createPlaylist("我的最爱");
    qDebug() << "Create playlist:" << result;
    
    // 测试添加歌曲到歌单
    result = playlist.addToPlaylist("我的最爱", "奇妙能力歌", "陈粒", "如也", "./resources/sound/奇妙能力歌.mp3", "./resources/pic/八度空间.jpg", "./resources/sound/奇妙能力歌.lrc", 258);
    qDebug() << "Add to playlist:" << result;
    
    // 测试获取所有歌单名称
    QStringList playlists = playlist.getAllPlaylistNames();
    qDebug() << "All playlists:" << playlists;
    
    // 测试获取收藏夹歌曲
    QStringList favorites = playlist.getFavoritesSongs();
    qDebug() << "Favorites songs count:" << favorites.count();
    
    // 清理资源
    playlist.cleanup();
}

// 使用说明：
// 1. 在main.cpp中初始化应用程序后调用测试函数（可选）
// 2. 在MainWindow类中集成PlaylistInterface
// 3. 添加收藏夹和歌单相关的UI控件和槽函数
// 4. 在程序启动时加载歌单，关闭时保存歌单

// 数据持久化说明：
// - 所有歌单数据保存在指定目录下的JSON文件中
// - 收藏夹保存为favorites.json
// - 自定义歌单以歌单名称为文件名保存

// 完整测试函数实现
void testPlaylistFunctions()
{
    // 创建播放列表接口
    PlaylistInterface playlist;
    playlist.initialize("./data/playlists");
    
    // 测试添加歌曲到收藏夹
    bool result = playlist.addToFavorites("奇妙能力歌", "陈粒", "如也", "./resources/sound/奇妙能力歌.mp3", "./resources/pic/八度空间.jpg", "./resources/sound/奇妙能力歌.lrc", 258);
    qDebug() << "Add to favorites:" << result;
    
    // 测试检查是否在收藏夹中
    bool inFavorites = playlist.isInFavorites("./resources/sound/奇妙能力歌.mp3");
    qDebug() << "Is in favorites:" << inFavorites;
    
    // 测试创建歌单
    result = playlist.createPlaylist("我的最爱");
    qDebug() << "Create playlist:" << result;
    
    // 测试添加歌曲到歌单
    result = playlist.addToPlaylist("我的最爱", "奇妙能力歌", "陈粒", "如也", "./resources/sound/奇妙能力歌.mp3", "./resources/pic/八度空间.jpg", "./resources/sound/奇妙能力歌.lrc", 258);
    qDebug() << "Add to playlist:" << result;
    
    // 测试获取所有歌单名称
    QStringList playlists = playlist.getAllPlaylistNames();
    qDebug() << "All playlists:" << playlists;
    
    // 测试获取收藏夹歌曲
    QStringList favorites = playlist.getFavoritesSongs();
    qDebug() << "Favorites songs count:" << favorites.count();
    
    // 清理资源
    playlist.cleanup();
}