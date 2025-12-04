/*
 * playlist_util.c
 * 播放列表工具函数实现
 * 提供播放列表的创建、管理、文件读写等功能
 */

#include "playlist_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#include <ctype.h>

// Windows平台下的mkdir函数别名
#define mkdir _mkdir

/**
 * @brief 安全的字符串复制函数
 * @param s 要复制的源字符串
 * @return 复制后的字符串指针，失败返回NULL
 */
char *safe_strdup(const char *s) {
    if (!s) return NULL;  // 空指针检查
    return strdup(s);     // 复制字符串
}

/**
 * @brief 构建播放列表文件路径
 * @param buf 存储结果的缓冲区
 * @param bufSize 缓冲区大小
 * @param data_dir 数据目录路径
 * @param name 播放列表名称
 * @return 成功返回true，失败返回false
 */
bool build_playlist_filepath(char *buf, size_t bufSize, const char *data_dir, const char *name) {
    if (!buf || !data_dir || !name) return false;  // 参数有效性检查
    
    char safe_name[256];
    // 清理文件名，确保安全
    if (!sanitize_filename(name, safe_name, sizeof(safe_name))) return false;
    
    // 格式化构建完整文件路径
    int n = _snprintf(buf, bufSize, "%s%c%s.csv", data_dir, PATH_SEPARATOR, safe_name);
    
    // 检查是否成功写入缓冲区且没有溢出
    return n >= 0 && (size_t)n < bufSize;
}

/**
 * @brief 清理文件名，去除不安全字符
 * @param name 原始文件名
 * @param out 输出缓冲区
 * @param outSize 输出缓冲区大小
 * @return 成功返回true，失败返回false
 */
bool sanitize_filename(const char *name, char *out, size_t outSize) {
    if (!name || !out || outSize == 0) return false;  // 参数有效性检查
    
    // 去除首尾空格
    const char *start = name;
    while (*start && isspace((unsigned char)*start)) start++;
    
    const char *end = name + strlen(name);
    while (end > start && isspace((unsigned char)*(end-1))) end--;
    
    size_t j = 0;
    // 遍历处理每个字符
    for (const char *p = start; p < end && j + 1 < outSize; ++p) {
        unsigned char c = (unsigned char)*p;
        
        // 允许字母数字、点、破折号、下划线；其他字符替换为 '_'
        if (isalnum(c) || c == '.' || c == '-' || c == '_') {
            out[j++] = (char)c;
        } else {
            out[j++] = '_';
        }
    }
    
    if (j == 0) return false;  // 处理后为空字符串
    out[j] = '\0';             // 添加字符串结束符
    
    return true;
}

/**
 * @brief 检查路径是否为目录
 * @param path 要检查的路径
 * @return 是目录返回true，否则返回false
 */
static bool path_is_directory(const char *path) {
    DWORD attrib = GetFileAttributesA(path);
    // 检查属性是否有效且是否为目录
    return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY);
}

/**
 * @brief 确保目录存在，如果不存在则创建
 * @param dir_path 目录路径
 * @return 成功返回true，失败返回false
 */
bool ensure_directory(const char *dir_path) {
    if (!dir_path) return false;  // 参数有效性检查
    
    // 如果目录已存在，直接返回成功
    if (path_is_directory(dir_path)) return true;
    
    // 迭代创建目录以避免递归问题
    char tmp[1024];
    size_t len = strlen(dir_path);
    if (len == 0 || len >= sizeof(tmp)) return false;  // 路径过长
    
    // 复制并规范化路径分隔符为反斜杠
    strncpy(tmp, dir_path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';  // 确保字符串结束
    
    for (size_t i = 0; i < strlen(tmp); ++i) {
        if (tmp[i] == '/') tmp[i] = '\\';  // 将正斜杠转换为反斜杠
    }
    
    // 处理带驱动器号的绝对路径（例如 C:\）或 UNC 路径（\\server\share）
    size_t pos = 0;
    if (strlen(tmp) >= 2 && tmp[1] == ':') {
        pos = 2;  // 跳过 "C:"
        if (tmp[2] == '\\') pos = 3;  // 跳过 "C:\"
    } else if (strlen(tmp) >= 2 && tmp[0] == '\\' && tmp[1] == '\\') {
        // UNC 路径：从 "\\server\share" 之后开始
        pos = 2;
        // 查找接下来的两个分隔符
        int sep_count = 0;
        for (size_t i = 2; i < strlen(tmp); ++i) {
            if (tmp[i] == '\\') {
                sep_count++;
                if (sep_count == 2) { pos = i + 1; break; }
            }
        }
        if (pos == 2) pos = strlen(tmp);  // 解析 UNC 失败，稍后退出
    }
    
    // 逐段创建目录
    for (size_t i = pos; i < strlen(tmp); ++i) {
        if (tmp[i] == '\\') {
            char save = tmp[i];
            tmp[i] = '\0';  // 临时截断路径
            
            // 如果此路径组件不存在则创建它
            if (!path_is_directory(tmp)) {
                if (mkdir(tmp) != 0) {
                    tmp[i] = save;  // 恢复原路径
                    return path_is_directory(tmp);  // 再次检查是否成功
                }
            }
            tmp[i] = save;  // 恢复原路径
        }
    }
    
    // 创建最终目录
    if (!path_is_directory(tmp)) {
        if (mkdir(tmp) != 0) {
            return path_is_directory(tmp);  // 再次检查是否成功
        }
    }
    
    return path_is_directory(tmp);  // 最终检查
}

/**
 * @brief 创建播放列表项
 * @param song 歌曲信息指针
 * @return 播放列表项指针，失败返回NULL
 */
PlaylistItem *create_playlist_item(SongInfo *song) {
    // 分配播放列表项内存并初始化为0
    PlaylistItem *item = calloc(1, sizeof(PlaylistItem));
    if (!item) return NULL;  // 内存分配失败
    
    item->song = song;  // 设置歌曲信息
    return item;
}

/**
 * @brief 创建播放列表结构
 * @param name 播放列表名称
 * @return 播放列表指针，失败返回NULL
 */
Playlist *create_playlist_structure(const char *name) {
    // 分配播放列表内存并初始化为0
    Playlist *playlist = calloc(1, sizeof(Playlist));
    if (!playlist) return NULL;  // 内存分配失败
    
    playlist->name = safe_strdup(name);  // 安全复制名称
    return playlist;
}

/**
 * @brief 向播放列表追加项
 * @param playlist 播放列表指针
 * @param item 要追加的播放列表项
 */
void append_item_to_playlist(Playlist *playlist, PlaylistItem *item) {
    if (!playlist || !item) return;  // 参数有效性检查
    
    if (!playlist->head) {
        // 播放列表为空，设置为第一个项
        playlist->head = item;
        playlist->tail = item;
    } else {
        // 追加到列表末尾
        playlist->tail->next = item;
        item->prev = playlist->tail;
        playlist->tail = item;
    }
    
    playlist->count++;  // 增加计数
}

/**
 * @brief 释放播放列表中的所有项
 * @param playlist 播放列表指针
 */
void free_playlist_items(Playlist *playlist) {
    if (!playlist) return;  // 参数有效性检查
    
    PlaylistItem *item = playlist->head;
    while (item) {
        PlaylistItem *next = item->next;  // 保存下一个项
        
        free_song_info(item->song);  // 释放歌曲信息
        free(item);                  // 释放播放列表项
        
        item = next;  // 移动到下一个项
    }
    
    // 重置播放列表状态
    playlist->head = NULL;
    playlist->tail = NULL;
    playlist->count = 0;
}

/**
 * @brief 将播放列表写入文件
 * @param filename 文件名
 * @param playlist 播放列表指针
 * @return 成功返回true，失败返回false
 */
bool write_playlist_file(const char *filename, Playlist *playlist) {
    if (!filename || !playlist) return false;  // 参数有效性检查
    
    FILE *fp = fopen(filename, "w");
    if (!fp) return false;  // 文件打开失败
    
    fprintf(fp, PLAYLIST_CSV_HEADER);  // 写入CSV头部
    
    PlaylistItem *it = playlist->head;
    while (it) {
        // 转义各个字段，确保CSV格式正确
        char *f0 = escape_csv_field(it->song->title ? it->song->title : "");
        char *f1 = escape_csv_field(it->song->artist ? it->song->artist : "");
        char *f2 = escape_csv_field(it->song->album ? it->song->album : "");
        char *f3 = escape_csv_field(it->song->file_path ? it->song->file_path : "");
        char *f4 = escape_csv_field(it->song->cover_path ? it->song->cover_path : "");
        char *f5 = escape_csv_field(it->song->lrc_path ? it->song->lrc_path : "");
        
        // 写入一行CSV数据
        fprintf(fp, "%s;%s;%s;%s;%s;%s;%d\n", f0, f1, f2, f3, f4, f5, it->song->duration);
        
        // 释放转义后的字段内存
        free(f0); free(f1); free(f2); free(f3); free(f4); free(f5);
        
        it = it->next;  // 移动到下一个项
    }
    
    fclose(fp);  // 关闭文件
    return true;
}

/**
 * @brief 检查歌曲是否在播放列表中
 * @param playlist 播放列表指针
 * @param file_path 歌曲文件路径
 * @return 存在返回true，不存在返回false
 */
bool is_song_in_playlist(Playlist *playlist, const char *file_path) {
    if (!playlist || !file_path) return false;  // 参数有效性检查
    
    PlaylistItem *item = playlist->head;
    while (item) {
        // 检查歌曲是否存在且路径匹配
        if (item->song && item->song->file_path && strcmp(item->song->file_path, file_path) == 0) {
            return true;
        }
        item = item->next;  // 移动到下一个项
    }
    
    return false;
}

/**
 * @brief 从播放列表中移除项
 * @param playlist 播放列表指针
 * @param item 要移除的项
 * @return 成功返回true，失败返回false
 */
bool remove_item_from_playlist(Playlist *playlist, PlaylistItem *item) {
    if (!playlist || !item) return false;  // 参数有效性检查
    
    // 处理前驱节点
    if (item->prev) {
        item->prev->next = item->next;
    } else {
        playlist->head = item->next;  // 是第一个项，更新头指针
    }
    
    // 处理后继节点
    if (item->next) {
        item->next->prev = item->prev;
    } else {
        playlist->tail = item->prev;  // 是最后一个项，更新尾指针
    }
    
    free_song_info(item->song);  // 释放歌曲信息
    free(item);                  // 释放播放列表项
    
    playlist->count--;  // 减少计数
    return true;
}

// ---------------- CSV解析辅助函数 ----------------

/**
 * @brief 解析CSV行
 * @param line 要解析的行
 * @param outFields 输出字段数组
 * @param maxFields 最大字段数
 * @return 成功返回解析的字段数，失败返回-1
 */
int parse_csv_line(const char *line, char **outFields, int maxFields) {
    if (!line || !outFields || maxFields <= 0) return -1;  // 参数有效性检查
    
    int field = 0;
    const char *p = line;
    
    while (*p && field < maxFields) {
        // 跳过前导空格
        while (*p == ' ' || *p == '\t') p++;
        
        // 分配字段缓冲区
        char *buf = calloc(1, 2048);
        if (!buf) return -1;  // 内存分配失败
        
        int bi = 0;
        if (*p == '"') {
            // 处理带引号的字段
            p++;  // 跳过引号
            
            while (*p) {
                if (*p == '"') {
                    if (p[1] == '"') {
                        // 转义引号 ("" -> ")
                        buf[bi++] = '"';
                        p += 2;
                        continue;
                    } else {
                        // 字段结束引号
                        p++;
                        break;
                    }
                }
                buf[bi++] = *p++;
            }
            
            // 跳过直到分号或行尾
            while (*p && *p != ';') p++;
            if (*p == ';') p++;
        } else {
            // 处理不带引号的字段
            while (*p && *p != ';') {
                buf[bi++] = *p++;
            }
            
            if (*p == ';') p++;
            
            // 去除尾随空格
            while (bi > 0 && (buf[bi-1] == ' ' || buf[bi-1] == '\t')) bi--;
        }
        
        buf[bi] = '\0';  // 添加字符串结束符
        outFields[field++] = buf;  // 保存字段
    }
    
    return field;  // 返回解析的字段数
}

/**
 * @brief 释放解析的CSV字段
 * @param fields 字段数组
 * @param count 字段数量
 */
void free_parsed_fields(char **fields, int count) {
    if (!fields) return;  // 参数有效性检查
    
    for (int i = 0; i < count; ++i) {
        free(fields[i]);  // 释放每个字段
    }
}

/**
 * @brief 转义CSV字段
 * @param field 要转义的字段
 * @return 转义后的字段，失败返回NULL
 */
char *escape_csv_field(const char *field) {
    if (!field) return strdup("");  // 空指针处理
    
    int needsQuote = 0;
    const char *p = field;
    
    // 检查是否需要引号（包含特殊字符）
    while (*p) {
        if (*p == ';' || *p == '"' || *p == '\n' || *p == '\r') {
            needsQuote = 1;
            break;
        }
        p++;
    }
    
    if (!needsQuote) {
        return strdup(field);  // 不需要转义，直接复制
    }
    
    // 分配最坏情况下的空间（每个引号需要两个）
    size_t len = strlen(field);
    char *out = malloc(len * 2 + 3);  // 2*len(转义引号) + "+"+\0
    if (!out) return NULL;  // 内存分配失败
    
    char *o = out;
    *o++ = '"';  // 开始引号
    
    for (p = field; *p; ++p) {
        if (*p == '"') {
            *o++ = '"';  // 转义引号
            *o++ = '"';
        } else {
            *o++ = *p;  // 普通字符
        }
    }
    
    *o++ = '"';  // 结束引号
    *o = '\0';   // 字符串结束符
    
    return out;
}