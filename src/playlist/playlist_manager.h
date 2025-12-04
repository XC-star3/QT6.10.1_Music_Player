// playlist_manager.h - 播放列表管理器头文件
#ifndef PLAYLIST_MANAGER_H
#define PLAYLIST_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

// 歌曲信息结构
typedef struct {
    char *title;        // 歌曲标题
    char *artist;       // 艺术家
    char *album;        // 专辑
    char *file_path;    // 文件路径
    char *cover_path;   // 封面路径
    char *lrc_path;     // 歌词路径
    int duration;       // 时长（秒）
} SongInfo;

// 歌单项结构
typedef struct PlaylistItem {
    SongInfo *song;                 // 歌曲信息
    struct PlaylistItem *next;      // 下一个歌单项
    struct PlaylistItem *prev;      // 上一个歌单项
} PlaylistItem;

// 歌单结构
typedef struct Playlist {
    char *name;             // 歌单名称
    PlaylistItem *head;     // 歌单头部
    PlaylistItem *tail;     // 歌单尾部
    int count;              // 歌曲数量
    struct Playlist *next;  // 指向下一个歌单（用于链表）
} Playlist;

// 收藏夹和歌单管理器
typedef struct PlaylistManager {
    Playlist *favorites;    // 收藏夹
    Playlist *playlists;    // 所有歌单列表
    int playlist_count;     // 歌单数量
    char *data_dir;         // 数据保存目录
} PlaylistManager;

// 初始化函数
PlaylistManager *playlist_manager_init(const char *data_directory);
void playlist_manager_free(PlaylistManager *manager);

// 收藏夹操作
bool add_to_favorites(PlaylistManager *manager, SongInfo *song);
bool remove_from_favorites(PlaylistManager *manager, const char *file_path);
bool is_in_favorites(PlaylistManager *manager, const char *file_path);
Playlist *get_favorites(PlaylistManager *manager);

// 歌单操作
Playlist *create_playlist(PlaylistManager *manager, const char *name);
bool delete_playlist(PlaylistManager *manager, const char *name);
Playlist *get_playlist(PlaylistManager *manager, const char *name);
bool add_to_playlist(Playlist *playlist, SongInfo *song);
bool remove_from_playlist(Playlist *playlist, const char *file_path);

// 文件系统操作
bool save_playlists(PlaylistManager *manager);
bool load_playlists(PlaylistManager *manager);

// 工具函数
SongInfo *create_song_info(const char *title, const char *artist, const char *album, 
                          const char *file_path, const char *cover_path, const char *lrc_path, 
                          int duration);
void free_song_info(SongInfo *song);

// 获取所有歌单名称
char **get_all_playlist_names(PlaylistManager *manager, int *count);
// 释放歌单名称数组
void free_playlist_names(char **names, int count);

#ifdef __cplusplus
}
#endif

#endif // PLAYLIST_MANAGER_H