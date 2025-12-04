/**
 * @file playlist_util.h
 * @brief 播放列表工具函数头文件
 * @details 提供播放列表的创建、管理、文件读写等功能的函数声明
 */

#ifndef PLAYLIST_UTIL_H
#define PLAYLIST_UTIL_H

#include "playlist_manager.h"
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 路径分隔符定义
 * @details Windows平台下的路径分隔符为反斜杠
 */
#define PATH_SEPARATOR '\\'

/**
 * @brief 播放列表CSV文件头部
 * @details 定义CSV文件的列名，分号分隔
 */
#define PLAYLIST_CSV_HEADER "title;artist;album;file_path;cover_path;lrc_path;duration\n"

/**
 * @brief 安全的字符串复制函数
 * @param s 要复制的源字符串
 * @return 复制后的字符串指针，失败返回NULL
 */
char *safe_strdup(const char *s);

/**
 * @brief 构建播放列表文件路径
 * @param buf 存储结果的缓冲区
 * @param bufSize 缓冲区大小
 * @param data_dir 数据目录路径
 * @param name 播放列表名称
 * @return 成功返回true，失败返回false
 */
bool build_playlist_filepath(char *buf, size_t bufSize, const char *data_dir, const char *name);

/**
 * @brief 确保目录存在，如果不存在则创建
 * @param dir_path 目录路径
 * @return 成功返回true，失败返回false
 */
bool ensure_directory(const char *dir_path);

/**
 * @brief 清理文件名，使其适合文件系统
 * @param name 用户提供的播放列表名称
 * @param out 输出缓冲区
 * @param outSize 输出缓冲区大小
 * @return 成功返回true，失败返回false
 * @details 将非法字符替换为'_'并去除首尾空格
 */
bool sanitize_filename(const char *name, char *out, size_t outSize);

/**
 * @brief 解析CSV行
 * @param line 要解析的CSV行
 * @param outFields 存储解析结果的字段数组
 * @param maxFields 最大字段数
 * @return 成功返回解析的字段数，失败返回-1
 * @details 解析分号分隔的CSV行，为每个字段分配内存，调用者需通过free_parsed_fields释放
 */
int parse_csv_line(const char *line, char **outFields, int maxFields);

/**
 * @brief 释放解析的CSV字段
 * @param fields 字段数组
 * @param count 字段数量
 * @details 释放由parse_csv_line分配的字段内存
 */
void free_parsed_fields(char **fields, int count);

/**
 * @brief 转义CSV字段
 * @param field 要转义的字段
 * @return 转义后的字段，失败返回NULL
 * @details 为CSV输出转义字段，分配内存，调用者需释放
 */
char *escape_csv_field(const char *field);

/**
 * @brief 创建播放列表项
 * @param song 歌曲信息指针
 * @return 播放列表项指针，失败返回NULL
 */
PlaylistItem *create_playlist_item(SongInfo *song);

/**
 * @brief 创建播放列表结构
 * @param name 播放列表名称
 * @return 播放列表指针，失败返回NULL
 */
Playlist *create_playlist_structure(const char *name);

/**
 * @brief 向播放列表追加项
 * @param playlist 播放列表指针
 * @param item 要追加的播放列表项
 */
void append_item_to_playlist(Playlist *playlist, PlaylistItem *item);

/**
 * @brief 释放播放列表中的所有项
 * @param playlist 播放列表指针
 */
void free_playlist_items(Playlist *playlist);

/**
 * @brief 将播放列表写入文件
 * @param filename 文件名
 * @param playlist 播放列表指针
 * @return 成功返回true，失败返回false
 */
bool write_playlist_file(const char *filename, Playlist *playlist);

/**
 * @brief 检查歌曲是否在播放列表中
 * @param playlist 播放列表指针
 * @param file_path 歌曲文件路径
 * @return 存在返回true，不存在返回false
 */
bool is_song_in_playlist(Playlist *playlist, const char *file_path);

/**
 * @brief 从播放列表中移除项
 * @param playlist 播放列表指针
 * @param item 要移除的项
 * @return 成功返回true，失败返回false
 */
bool remove_item_from_playlist(Playlist *playlist, PlaylistItem *item);

#ifdef __cplusplus
}
#endif

#endif // PLAYLIST_UTIL_H