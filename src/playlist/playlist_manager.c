#include "playlist_manager.h"
#include "playlist_util.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

// 初始化播放列表管理器
PlaylistManager *playlist_manager_init(const char *data_directory) {
    PlaylistManager *manager = calloc(1, sizeof(PlaylistManager));
    if (!manager) return NULL;

    manager->data_dir = safe_strdup(data_directory);
    
    if (!ensure_directory(data_directory)) {
        free(manager->data_dir);
        free(manager);
        return NULL;
    }
    
    manager->favorites = create_playlist_structure("favorites");
    if (!manager->favorites) {
        free(manager->data_dir);
        free(manager);
        return NULL;
    }
    
    manager->playlists = NULL;
    manager->playlist_count = 0;
    
    load_playlists(manager);
    
    return manager;
}

// 释放播放列表管理器
void playlist_manager_free(PlaylistManager *manager) {
    if (!manager) return;
    
    // 释放收藏夹
    if (manager->favorites) {
        free_playlist_items(manager->favorites);
        free(manager->favorites->name);
        free(manager->favorites);
    }
    
    // 释放所有歌单
    Playlist *playlist = manager->playlists;
    while (playlist) {
        Playlist *next = playlist->next;
        free_playlist_items(playlist);
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
    SongInfo *song = calloc(1, sizeof(SongInfo));
    if (!song) return NULL;

    song->title = title ? safe_strdup(title) : NULL;
    song->artist = artist ? safe_strdup(artist) : NULL;
    song->album = album ? safe_strdup(album) : NULL;
    song->file_path = file_path ? safe_strdup(file_path) : NULL;
    song->cover_path = cover_path ? safe_strdup(cover_path) : NULL;
    song->lrc_path = lrc_path ? safe_strdup(lrc_path) : NULL;
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
    if (!manager || !manager->favorites || !song || !song->file_path || is_in_favorites(manager, song->file_path)) {
        return false;
    }
    
    PlaylistItem *item = create_playlist_item(song);
    if (!item) return false;
    
    append_item_to_playlist(manager->favorites, item);
    save_playlists(manager);
    
    return true;
}

// 获取所有歌单名称
char **get_all_playlist_names(PlaylistManager *manager, int *count) {
    if (!manager || !count) return NULL;
    
    *count = manager->playlist_count;
    if (*count == 0) return NULL;
    
    char **names = malloc(sizeof(char *) * (*count));
    if (!names) return NULL;
    
    Playlist *playlist = manager->playlists;
    int i = 0;
    while (playlist && i < *count) {
    names[i] = safe_strdup(playlist->name);
        if (!names[i]) {
            for (int j = 0; j < i; j++) {
                free(names[j]);
            }
            free(names);
            return NULL;
        }
        playlist = playlist->next;
        i++;
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

// 从收藏夹移除
bool remove_from_favorites(PlaylistManager *manager, const char *file_path) {
    if (!manager || !manager->favorites || !file_path) {
        return false;
    }
    
    PlaylistItem *item = manager->favorites->head;
    while (item) {
        if (item->song && item->song->file_path && strcmp(item->song->file_path, file_path) == 0) {
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
            save_playlists(manager);
            
            return true;
        }
        item = item->next;
    }
    
    return false;
}

// 检查是否在收藏夹中
bool is_in_favorites(PlaylistManager *manager, const char *file_path) {
    return manager && manager->favorites && file_path ? 
           is_song_in_playlist(manager->favorites, file_path) : false;
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
        playlist = playlist->next;
    }
    
    // 创建新歌单
    Playlist *new_playlist = create_playlist_structure(name);
    if (!new_playlist) return NULL;
    
    new_playlist->next = manager->playlists;
    manager->playlists = new_playlist;
    manager->playlist_count++;
    // 保存新建的歌单以便立即持久化
    save_playlists(manager);
    
    return new_playlist;
}

// 删除歌单
bool delete_playlist(PlaylistManager *manager, const char *name) {
    if (!manager || !name) return false;
    
    Playlist *playlist = manager->playlists;
    Playlist *prev = NULL;
    
    while (playlist) {
        if (strcmp(playlist->name, name) == 0) {
            if (prev) {
                prev->next = playlist->next;
            } else {
                manager->playlists = playlist->next;
            }
            
            // 释放歌单内容
            free_playlist_items(playlist);
            
            free(playlist->name);
            free(playlist);
            manager->playlist_count--;
            
            // 删除歌单文件
            char filename[1024];
            if (build_playlist_filepath(filename, sizeof(filename), manager->data_dir, name)) {
                remove(filename);
            }
            
            return true;
        }
        prev = playlist;
        playlist = playlist->next;
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
        playlist = playlist->next;
    }
    
    return NULL;
}

// 添加到歌单
bool add_to_playlist(Playlist *playlist, SongInfo *song) {
    if (!playlist || !song || !song->file_path || is_song_in_playlist(playlist, song->file_path)) {
        return false;
    }
    
    PlaylistItem *new_item = create_playlist_item(song);
    if (!new_item) return false;
    
    append_item_to_playlist(playlist, new_item);
    return true;
}

// 从歌单移除
bool remove_from_playlist(Playlist *playlist, const char *file_path) {
    if (!playlist || !file_path) return false;
    
    PlaylistItem *item = playlist->head;
    while (item) {
        if (item->song && item->song->file_path && strcmp(item->song->file_path, file_path) == 0) {
            return remove_item_from_playlist(playlist, item);
        }
        item = item->next;
    }
    
    return false;
}

// 保存歌单到文件系统
bool save_playlists(PlaylistManager *manager) {
    if (!manager || !ensure_directory(manager->data_dir)) return false;
    
    // 保存收藏夹
    char favorites_file[1024];
    if (!build_playlist_filepath(favorites_file, sizeof(favorites_file), manager->data_dir, "favorites")) return false;
    
    if (!write_playlist_file(favorites_file, manager->favorites)) return false;
    
    // 保存所有用户歌单
    Playlist *playlist = manager->playlists;
    while (playlist) {
        char playlist_file[1024];
        if (build_playlist_filepath(playlist_file, sizeof(playlist_file), manager->data_dir, playlist->name)) {
            write_playlist_file(playlist_file, playlist);
        }
        
        playlist = playlist->next;
    }
    
    return true;
}

// 从文件系统加载歌单
bool load_playlists(PlaylistManager *manager) {
    if (!manager) return false;
    
    // 加载收藏夹
    char favorites_file[1024];
    if (build_playlist_filepath(favorites_file, sizeof(favorites_file), manager->data_dir, "favorites")) {
        FILE *fp = fopen(favorites_file, "r");
        if (fp) {
            char line[2048];
            // 跳过CSV头
            if (fgets(line, sizeof(line), fp)) {
                while (fgets(line, sizeof(line), fp)) {
                    // parse line into fields
                    char *fields[8] = {0};
                    int fc = parse_csv_line(line, fields, 8);
                    if (fc <= 0) { free_parsed_fields(fields, fc); continue; }
                    char *title = fc > 0 ? fields[0] : NULL;
                    char *artist = fc > 1 ? fields[1] : NULL;
                    char *album = fc > 2 ? fields[2] : NULL;
                    char *file_path = fc > 3 ? fields[3] : NULL;
                    char *cover_path = fc > 4 ? fields[4] : NULL;
                    char *lrc_path = fc > 5 ? fields[5] : NULL;
                    char *duration_str = fc > 6 ? fields[6] : NULL;
                    int duration = duration_str ? atoi(duration_str) : 0;

                    if (file_path && strlen(file_path) > 0) {
                        SongInfo *song_info = create_song_info(
                            title && strlen(title) > 0 ? title : NULL,
                            artist && strlen(artist) > 0 ? artist : NULL,
                            album && strlen(album) > 0 ? album : NULL,
                            file_path,
                            cover_path && strlen(cover_path) > 0 ? cover_path : NULL,
                            lrc_path && strlen(lrc_path) > 0 ? lrc_path : NULL,
                            duration
                        );

                        if (song_info && !is_in_favorites(manager, file_path)) {
                            PlaylistItem *item = create_playlist_item(song_info);
                            if (item) {
                                append_item_to_playlist(manager->favorites, item);
                            } else {
                                free_song_info(song_info);
                            }
                        } else if (song_info) {
                            free_song_info(song_info);
                        }
                    }
                    free_parsed_fields(fields, fc);
                }
            }
            fclose(fp);
        }
    }

    // 加载用户歌单（扫描目录中的所有 csv 文件，排除 favorites.csv）
    char pattern[1024];
    int pn = _snprintf(pattern, sizeof(pattern), "%s%c*.csv", manager->data_dir, PATH_SEPARATOR);
    if (pn <= 0 || (size_t)pn >= sizeof(pattern)) return true;

    WIN32_FIND_DATAA findData;
    HANDLE hFind = FindFirstFileA(pattern, &findData);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            if (strcmp(findData.cFileName, "favorites.csv") == 0) continue;

            // get name without .csv
            char nameOnly[256];
            strncpy(nameOnly, findData.cFileName, sizeof(nameOnly)-1);
            nameOnly[sizeof(nameOnly)-1] = '\0';
            char *dot = strrchr(nameOnly, '.');
            if (dot) *dot = '\0';

            // create playlist structure and load its contents
            Playlist *pl = create_playlist_structure(nameOnly);
            if (!pl) continue;
            // append to manager->playlists
            pl->next = manager->playlists;
            manager->playlists = pl;
            manager->playlist_count++;

            // read file
            char filepath[1024];
            if (!build_playlist_filepath(filepath, sizeof(filepath), manager->data_dir, nameOnly)) continue;
            FILE *fp2 = fopen(filepath, "r");
            if (!fp2) continue;
            char line2[2048];
            // skip header
            if (fgets(line2, sizeof(line2), fp2)) {
                while (fgets(line2, sizeof(line2), fp2)) {
                    char *fields2[8] = {0};
                    int fc2 = parse_csv_line(line2, fields2, 8);
                    if (fc2 <= 0) { free_parsed_fields(fields2, fc2); continue; }
                    char *title = fc2 > 0 ? fields2[0] : NULL;
                    char *artist = fc2 > 1 ? fields2[1] : NULL;
                    char *album = fc2 > 2 ? fields2[2] : NULL;
                    char *file_path = fc2 > 3 ? fields2[3] : NULL;
                    char *cover_path = fc2 > 4 ? fields2[4] : NULL;
                    char *lrc_path = fc2 > 5 ? fields2[5] : NULL;
                    char *duration_str = fc2 > 6 ? fields2[6] : NULL;
                    int duration = duration_str ? atoi(duration_str) : 0;

                    if (file_path && strlen(file_path) > 0) {
                        SongInfo *song_info = create_song_info(
                            title && strlen(title) > 0 ? title : NULL,
                            artist && strlen(artist) > 0 ? artist : NULL,
                            album && strlen(album) > 0 ? album : NULL,
                            file_path,
                            cover_path && strlen(cover_path) > 0 ? cover_path : NULL,
                            lrc_path && strlen(lrc_path) > 0 ? lrc_path : NULL,
                            duration
                        );
                        if (song_info) {
                            PlaylistItem *it = create_playlist_item(song_info);
                            if (it) append_item_to_playlist(pl, it);
                            else free_song_info(song_info);
                        }
                    }
                    free_parsed_fields(fields2, fc2);
                }
            }
            fclose(fp2);

        } while (FindNextFileA(hFind, &findData));
        FindClose(hFind);
    }
    
    return true;
}

// safe_strdup is defined earlier in the file.