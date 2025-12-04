#ifndef PLAYLIST_UTIL_H
#define PLAYLIST_UTIL_H

#include "playlist_manager.h"
#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PATH_SEPARATOR '\\'
#define PLAYLIST_CSV_HEADER "title;artist;album;file_path;cover_path;lrc_path;duration\n"

// helpers extracted from playlist_manager.c
char *safe_strdup(const char *s);
bool build_playlist_filepath(char *buf, size_t bufSize, const char *data_dir, const char *name);
bool ensure_directory(const char *dir_path);
// Make a filesystem-safe filename from a user-provided playlist name.
// Replaces illegal characters with '_' and trims whitespace. Returns true on success.
bool sanitize_filename(const char *name, char *out, size_t outSize);
// CSV helpers
// Parse a semicolon-separated CSV line into fields. Caller provides an array of char* (outFields)
// with capacity maxFields; function will allocate strings for each field (caller must free via free_parsed_fields).
// Returns number of fields parsed, or -1 on error.
int parse_csv_line(const char *line, char **outFields, int maxFields);
// Free parsed fields
void free_parsed_fields(char **fields, int count);
// Escape a field for CSV output (allocates returned string, caller must free)
char *escape_csv_field(const char *field);
PlaylistItem *create_playlist_item(SongInfo *song);
Playlist *create_playlist_structure(const char *name);
void append_item_to_playlist(Playlist *playlist, PlaylistItem *item);
void free_playlist_items(Playlist *playlist);
bool write_playlist_file(const char *filename, Playlist *playlist);
bool is_song_in_playlist(Playlist *playlist, const char *file_path);
bool remove_item_from_playlist(Playlist *playlist, PlaylistItem *item);

#ifdef __cplusplus
}
#endif

#endif // PLAYLIST_UTIL_H
