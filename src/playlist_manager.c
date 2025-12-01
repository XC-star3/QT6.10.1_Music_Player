#include "playlist_manager.h"
#include <direct.h>
#include <errno.h>

// 确保目录存在
static bool ensure_directory(const char *dir_path) {
    if (_mkdir(dir_path) == 0) {
        return true;  // 成功创建目录
    }
    if (errno == EEXIST) {
        return true;  // 目录已存在
    }
    return false;  // 创建失败
}

// 初始化歌单项
static PlaylistItem *create_playlist_item(SongInfo *song) {
    PlaylistItem *item = (PlaylistItem *)malloc(sizeof(PlaylistItem));
    if (!item) return NULL;
    
    item->song = song;
    item->next = NULL;
    item->prev = NULL;
    
    return item;
}

// 初始化歌单
static Playlist *create_playlist_structure(const char *name) {
    Playlist *playlist = (Playlist *)malloc(sizeof(Playlist));
    if (!playlist) return NULL;
    
    playlist->name = strdup(name);
    playlist->head = NULL;
    playlist->tail = NULL;
    playlist->count = 0;
    
    return playlist;
}

// 初始化播放列表管理器
PlaylistManager *playlist_manager_init(const char *data_directory) {
    PlaylistManager *manager = (PlaylistManager *)malloc(sizeof(PlaylistManager));
    if (!manager) return NULL;
    
    manager->data_dir = strdup(data_directory);
    
    // 创建数据目录
    if (!ensure_directory(data_directory)) {
        free(manager->data_dir);
        free(manager);
        return NULL;
    }
    
    // 初始化收藏夹
    manager->favorites = create_playlist_structure("favorites");
    if (!manager->favorites) {
        free(manager->data_dir);
        free(manager);
        return NULL;
    }
    
    // 初始化歌单列表
    manager->playlists = NULL;
    manager->playlist_count = 0;
    
    // 加载保存的歌单
    load_playlists(manager);
    
    return manager;
}

// 释放播放列表管理器
void playlist_manager_free(PlaylistManager *manager) {
    if (!manager) return;
    
    // 释放收藏夹
    if (manager->favorites) {
        PlaylistItem *item = manager->favorites->head;
        while (item) {
            PlaylistItem *next = item->next;
            free_song_info(item->song);
            free(item);
            item = next;
        }
        free(manager->favorites->name);
        free(manager->favorites);
    }
    
    // 释放所有歌单
    Playlist *playlist = manager->playlists;
    while (playlist) {
        Playlist *next = playlist->next;
        PlaylistItem *item = playlist->head;
        while (item) {
            PlaylistItem *item_next = item->next;
            free_song_info(item->song);
            free(item);
            item = item_next;
        }
        free(playlist->name);
        free(playlist);
        playlist = next;
    }
    
    free(manager->data_dir);
    free(manager);
}

// 创建歌曲信息
SongInfo *create_song_info(const char *title, const char *artist, const char *album, 
                          const char *file_path, const char *cover_path, const char *lrc_path, 
                          int duration) {
    SongInfo *song = (SongInfo *)malloc(sizeof(SongInfo));
    if (!song) return NULL;
    
    song->title = title ? strdup(title) : NULL;
    song->artist = artist ? strdup(artist) : NULL;
    song->album = album ? strdup(album) : NULL;
    song->file_path = file_path ? strdup(file_path) : NULL;
    song->cover_path = cover_path ? strdup(cover_path) : NULL;
    song->lrc_path = lrc_path ? strdup(lrc_path) : NULL;
    song->duration = duration;
    
    return song;
}

// 释放歌曲信息
void free_song_info(SongInfo *song) {
    if (!song) return;
    
    free(song->title);
    free(song->artist);
    free(song->album);
    free(song->file_path);
    free(song->cover_path);
    free(song->lrc_path);
    free(song);
}

// 添加到收藏夹
bool add_to_favorites(PlaylistManager *manager, SongInfo *song) {
    if (!manager || !manager->favorites || !song || !song->file_path) {
        return false;
    }
    
    // 检查是否已存在
    if (is_in_favorites(manager, song->file_path)) {
        return false;
    }
    
    // 创建歌单项
    PlaylistItem *item = create_playlist_item(song);
    if (!item) return false;
    
    // 添加到收藏夹
    if (!manager->favorites->head) {
        manager->favorites->head = item;
        manager->favorites->tail = item;
    } else {
        manager->favorites->tail->next = item;
        item->prev = manager->favorites->tail;
        manager->favorites->tail = item;
    }
    
    manager->favorites->count++;
    
    // 保存收藏夹
    save_playlists(manager);
    
    return true;
}

// 从收藏夹移除
bool remove_from_favorites(PlaylistManager *manager, const char *file_path) {
    if (!manager || !manager->favorites || !file_path) {
        return false;
    }
    
    PlaylistItem *item = manager->favorites->head;
    while (item) {
        if (item->song->file_path && strcmp(item->song->file_path, file_path) == 0) {
            // 从链表中移除
            if (item->prev) {
                item->prev->next = item->next;
            } else {
                manager->favorites->head = item->next;
            }
            
            if (item->next) {
                item->next->prev = item->prev;
            } else {
                manager->favorites->tail = item->prev;
            }
            
            free_song_info(item->song);
            free(item);
            
            manager->favorites->count--;
            
            // 保存收藏夹
            save_playlists(manager);
            
            return true;
        }
        item = item->next;
    }
    
    return false;
}

// 检查是否在收藏夹中
bool is_in_favorites(PlaylistManager *manager, const char *file_path) {
    if (!manager || !manager->favorites || !file_path) {
        return false;
    }
    
    PlaylistItem *item = manager->favorites->head;
    while (item) {
        if (item->song->file_path && strcmp(item->song->file_path, file_path) == 0) {
            return true;
        }
        item = item->next;
    }
    
    return false;
}

// 获取收藏夹
Playlist *get_favorites(PlaylistManager *manager) {
    return manager ? manager->favorites : NULL;
}

// 创建歌单
Playlist *create_playlist(PlaylistManager *manager, const char *name) {
    if (!manager || !name) return NULL;
    
    // 检查歌单是否已存在
    Playlist *playlist = manager->playlists;
    while (playlist) {
        if (strcmp(playlist->name, name) == 0) {
            return playlist;  // 歌单已存在
        }
        playlist = (Playlist *)playlist->next;  // 强制转换用于链表遍历
    }
    
    // 创建新歌单
    Playlist *new_playlist = create_playlist_structure(name);
    if (!new_playlist) return NULL;
    
    // 添加到歌单列表
    new_playlist->next = (PlaylistItem *)manager->playlists;  // 强制转换
    manager->playlists = new_playlist;
    manager->playlist_count++;
    
    return new_playlist;
}

// 删除歌单
bool delete_playlist(PlaylistManager *manager, const char *name) {
    if (!manager || !name) return false;
    
    Playlist *playlist = manager->playlists;
    Playlist *prev = NULL;
    
    while (playlist) {
        if (strcmp(playlist->name, name) == 0) {
            // 从链表中移除
            if (prev) {
                prev->next = playlist->next;
            } else {
                manager->playlists = (Playlist *)playlist->next;
            }
            
            // 释放歌单内容
            PlaylistItem *item = playlist->head;
            while (item) {
                PlaylistItem *next = item->next;
                free_song_info(item->song);
                free(item);
                item = next;
            }
            
            free(playlist->name);
            free(playlist);
            
            manager->playlist_count--;
            
            // 删除歌单文件
            char filename[1024];
            snprintf(filename, sizeof(filename), "%s\\%s.json", manager->data_dir, name);
            remove(filename);
            
            return true;
        }
        prev = playlist;
        playlist = (Playlist *)playlist->next;
    }
    
    return false;
}

// 获取歌单
Playlist *get_playlist(PlaylistManager *manager, const char *name) {
    if (!manager || !name) return NULL;
    
    Playlist *playlist = manager->playlists;
    while (playlist) {
        if (strcmp(playlist->name, name) == 0) {
            return playlist;
        }
        playlist = (Playlist *)playlist->next;
    }
    
    return NULL;
}

// 添加到歌单
bool add_to_playlist(Playlist *playlist, SongInfo *song) {
    if (!playlist || !song || !song->file_path) return false;
    
    // 检查是否已存在
    PlaylistItem *item = playlist->head;
    while (item) {
        if (item->song->file_path && strcmp(item->song->file_path, song->file_path) == 0) {
            return false;  // 已存在
        }
        item = item->next;
    }
    
    // 创建新的歌单项
    PlaylistItem *new_item = create_playlist_item(song);
    if (!new_item) return false;
    
    // 添加到歌单
    if (!playlist->head) {
        playlist->head = new_item;
        playlist->tail = new_item;
    } else {
        playlist->tail->next = new_item;
        new_item->prev = playlist->tail;
        playlist->tail = new_item;
    }
    
    playlist->count++;
    
    return true;
}

// 从歌单移除
bool remove_from_playlist(Playlist *playlist, const char *file_path) {
    if (!playlist || !file_path) return false;
    
    PlaylistItem *item = playlist->head;
    while (item) {
        if (item->song->file_path && strcmp(item->song->file_path, file_path) == 0) {
            // 从链表中移除
            if (item->prev) {
                item->prev->next = item->next;
            } else {
                playlist->head = item->next;
            }
            
            if (item->next) {
                item->next->prev = item->prev;
            } else {
                playlist->tail = item->prev;
            }
            
            free_song_info(item->song);
            free(item);
            
            playlist->count--;
            
            return true;
        }
        item = item->next;
    }
    
    return false;
}

// 保存歌单到文件系统
bool save_playlists(PlaylistManager *manager) {
    if (!manager) return false;
    
    // 保存收藏夹
    char favorites_file[1024];
    snprintf(favorites_file, sizeof(favorites_file), "%s\\favorites.json", manager->data_dir);
    
    FILE *fp = fopen(favorites_file, "w");
    if (fp) {
        fprintf(fp, "{\n\t\"name\": \"favorites\",\n\t\"count\": %d,\n\t\"songs\": [\n", manager->favorites->count);
        
        PlaylistItem *item = manager->favorites->head;
        while (item) {
            fprintf(fp, "\t\t{\n");
            fprintf(fp, "\t\t\t\"title\": \"%s\",\n", item->song->title ? item->song->title : "");
            fprintf(fp, "\t\t\t\"artist\": \"%s\",\n", item->song->artist ? item->song->artist : "");
            fprintf(fp, "\t\t\t\"album\": \"%s\",\n", item->song->album ? item->song->album : "");
            fprintf(fp, "\t\t\t\"file_path\": \"%s\",\n", item->song->file_path ? item->song->file_path : "");
            fprintf(fp, "\t\t\t\"cover_path\": \"%s\",\n", item->song->cover_path ? item->song->cover_path : "");
            fprintf(fp, "\t\t\t\"lrc_path\": \"%s\",\n", item->song->lrc_path ? item->song->lrc_path : "");
            fprintf(fp, "\t\t\t\"duration\": %d\n", item->song->duration);
            fprintf(fp, "\t\t}");
            
            item = item->next;
            if (item) {
                fprintf(fp, ",");
            }
            fprintf(fp, "\n");
        }
        
        fprintf(fp, "\t]\n}");
        fclose(fp);
    } else {
        return false;
    }
    
    return true;
}

// 从文件系统加载歌单
bool load_playlists(PlaylistManager *manager) {
    if (!manager) return false;
    
    // 加载收藏夹
    char favorites_file[1024];
    snprintf(favorites_file, sizeof(favorites_file), "%s\\favorites.json", manager->data_dir);
    
    FILE *fp = fopen(favorites_file, "r");
    if (fp) {
        // 这里简化处理，实际项目中应该使用JSON解析库
        // 这里仅作为示例，实际使用时需要完整的JSON解析
        fclose(fp);
    }
    
    return true;
}

// 获取所有歌单名称
char **get_all_playlist_names(PlaylistManager *manager, int *count) {
    if (!manager || !count) return NULL;
    
    *count = manager->playlist_count + 1;  // 加上收藏夹
    
    char **names = (char **)malloc(sizeof(char *) * (*count));
    if (!names) {
        *count = 0;
        return NULL;
    }
    
    // 添加收藏夹
    names[0] = strdup("favorites");
    
    // 添加所有歌单
    int index = 1;
    Playlist *playlist = manager->playlists;
    while (playlist && index < *count) {
        names[index] = strdup(playlist->name);
        index++;
        playlist = (Playlist *)playlist->next;
    }
    
    return names;
}

// 释放歌单名称数组
void free_playlist_names(char **names, int count) {
    if (!names) return;
    
    for (int i = 0; i < count; i++) {
        if (names[i]) {
            free(names[i]);
        }
    }
    
    free(names);
}