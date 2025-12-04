#include "playlist_util.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#include <ctype.h>

#define mkdir _mkdir

char *safe_strdup(const char *s) {
    if (!s) return NULL;
    return strdup(s);
}

bool build_playlist_filepath(char *buf, size_t bufSize, const char *data_dir, const char *name) {
    if (!buf || !data_dir || !name) return false;
    char safe_name[256];
    if (!sanitize_filename(name, safe_name, sizeof(safe_name))) return false;
    int n = _snprintf(buf, bufSize, "%s%c%s.csv", data_dir, PATH_SEPARATOR, safe_name);
    return n >= 0 && (size_t)n < bufSize;
}

bool sanitize_filename(const char *name, char *out, size_t outSize) {
    if (!name || !out || outSize == 0) return false;
    // Trim leading/trailing spaces
    const char *start = name;
    while (*start && isspace((unsigned char)*start)) start++;
    const char *end = name + strlen(name);
    while (end > start && isspace((unsigned char)*(end-1))) end--;

    size_t j = 0;
    for (const char *p = start; p < end && j + 1 < outSize; ++p) {
        unsigned char c = (unsigned char)*p;
        // Allow alnum, dot, dash, underscore; replace others with '_'
        if (isalnum(c) || c == '.' || c == '-' || c == '_') {
            out[j++] = (char)c;
        } else {
            out[j++] = '_';
        }
    }
    if (j == 0) return false;
    out[j] = '\0';
    return true;
}

static bool path_is_directory(const char *path) {
    DWORD attrib = GetFileAttributesA(path);
    return attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY);
}

bool ensure_directory(const char *dir_path) {
    if (!dir_path) return false;
    if (path_is_directory(dir_path)) return true;

    // Create directories iteratively to avoid recursion issues
    char tmp[1024];
    size_t len = strlen(dir_path);
    if (len == 0 || len >= sizeof(tmp)) return false;

    // copy and normalize separators to backslash
    strncpy(tmp, dir_path, sizeof(tmp) - 1);
    tmp[sizeof(tmp) - 1] = '\0';
    for (size_t i = 0; i < strlen(tmp); ++i) {
        if (tmp[i] == '/') tmp[i] = '\\';
    }

    // Handle absolute paths with drive letter (e.g., C:\) or UNC paths (\\server\share)
    size_t pos = 0;
    if (strlen(tmp) >= 2 && tmp[1] == ':') {
        pos = 2; // skip "C:"
        if (tmp[2] == '\\') pos = 3; // skip "C:\"
    } else if (strlen(tmp) >= 2 && tmp[0] == '\\' && tmp[1] == '\\') {
        // UNC path: start after "\\server\share"
        pos = 2;
        // find next two separators
        int sep_count = 0;
        for (size_t i = 2; i < strlen(tmp); ++i) {
            if (tmp[i] == '\\') {
                sep_count++;
                if (sep_count == 2) { pos = i + 1; break; }
            }
        }
        if (pos == 2) pos = strlen(tmp); // failed to parse UNC, bail later
    }

    for (size_t i = pos; i < strlen(tmp); ++i) {
        if (tmp[i] == '\\') {
            char save = tmp[i];
            tmp[i] = '\0';
            // create this path component if it doesn't exist
            if (!path_is_directory(tmp)) {
                if (mkdir(tmp) != 0) {
                    tmp[i] = save;
                    return path_is_directory(tmp);
                }
            }
            tmp[i] = save;
        }
    }

    // create the final directory
    if (!path_is_directory(tmp)) {
        if (mkdir(tmp) != 0) {
            return path_is_directory(tmp);
        }
    }

    return path_is_directory(tmp);
}

PlaylistItem *create_playlist_item(SongInfo *song) {
    PlaylistItem *item = calloc(1, sizeof(PlaylistItem));
    if (!item) return NULL;
    item->song = song;
    return item;
}

Playlist *create_playlist_structure(const char *name) {
    Playlist *playlist = calloc(1, sizeof(Playlist));
    if (!playlist) return NULL;
    playlist->name = safe_strdup(name);
    return playlist;
}

void append_item_to_playlist(Playlist *playlist, PlaylistItem *item) {
    if (!playlist || !item) return;
    if (!playlist->head) {
        playlist->head = item;
        playlist->tail = item;
    } else {
        playlist->tail->next = item;
        item->prev = playlist->tail;
        playlist->tail = item;
    }
    playlist->count++;
}

void free_playlist_items(Playlist *playlist) {
    if (!playlist) return;
    PlaylistItem *item = playlist->head;
    while (item) {
        PlaylistItem *next = item->next;
        free_song_info(item->song);
        free(item);
        item = next;
    }
    playlist->head = NULL;
    playlist->tail = NULL;
    playlist->count = 0;
}

bool write_playlist_file(const char *filename, Playlist *playlist) {
    if (!filename || !playlist) return false;
    FILE *fp = fopen(filename, "w");
    if (!fp) return false;
    fprintf(fp, PLAYLIST_CSV_HEADER);
    PlaylistItem *it = playlist->head;
    while (it) {
        char *f0 = escape_csv_field(it->song->title ? it->song->title : "");
        char *f1 = escape_csv_field(it->song->artist ? it->song->artist : "");
        char *f2 = escape_csv_field(it->song->album ? it->song->album : "");
        char *f3 = escape_csv_field(it->song->file_path ? it->song->file_path : "");
        char *f4 = escape_csv_field(it->song->cover_path ? it->song->cover_path : "");
        char *f5 = escape_csv_field(it->song->lrc_path ? it->song->lrc_path : "");
        fprintf(fp, "%s;%s;%s;%s;%s;%s;%d\n", f0, f1, f2, f3, f4, f5, it->song->duration);
        free(f0); free(f1); free(f2); free(f3); free(f4); free(f5);
        it = it->next;
    }
    fclose(fp);
    return true;
}

bool is_song_in_playlist(Playlist *playlist, const char *file_path) {
    if (!playlist || !file_path) return false;
    PlaylistItem *item = playlist->head;
    while (item) {
        if (item->song && item->song->file_path && strcmp(item->song->file_path, file_path) == 0) {
            return true;
        }
        item = item->next;
    }
    return false;
}

bool remove_item_from_playlist(Playlist *playlist, PlaylistItem *item) {
    if (!playlist || !item) return false;
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

// ---------------- CSV parsing helpers ----------------
// Simple CSV parser for semicolon-separated fields with optional double-quote quoting.
// Supports quoted fields that may contain semicolons and escaped quotes ("" -> ").
int parse_csv_line(const char *line, char **outFields, int maxFields) {
    if (!line || !outFields || maxFields <= 0) return -1;
    int field = 0;
    const char *p = line;
    while (*p && field < maxFields) {
        // skip leading spaces
        while (*p == ' ' || *p == '\t') p++;
        char *buf = calloc(1, 2048);
        if (!buf) return -1;
        int bi = 0;
        if (*p == '"') {
            // quoted field
            p++; // skip quote
            while (*p) {
                if (*p == '"') {
                    if (p[1] == '"') { // escaped quote
                        buf[bi++] = '"'; p += 2; continue;
                    } else { p++; break; }
                }
                buf[bi++] = *p++;
            }
            // after quoted field, skip until ; or end
            while (*p && *p != ';') p++;
            if (*p == ';') p++;
        } else {
            // unquoted field
            while (*p && *p != ';') {
                buf[bi++] = *p++;
            }
            if (*p == ';') p++;
            // trim trailing space
            while (bi > 0 && (buf[bi-1] == ' ' || buf[bi-1] == '\t')) bi--;
        }
        buf[bi] = '\0';
        outFields[field++] = buf;
    }
    return field;
}

void free_parsed_fields(char **fields, int count) {
    if (!fields) return;
    for (int i = 0; i < count; ++i) {
        free(fields[i]);
    }
}

char *escape_csv_field(const char *field) {
    if (!field) return strdup("");
    int needsQuote = 0;
    const char *p = field;
    while (*p) { if (*p == ';' || *p == '"' || *p == '\n' || *p == '\r') { needsQuote = 1; break; } p++; }
    if (!needsQuote) return strdup(field);
    // allocate worst case (each quote doubles)
    size_t len = strlen(field);
    char *out = malloc(len * 2 + 3);
    if (!out) return NULL;
    char *o = out;
    *o++ = '"';
    for (p = field; *p; ++p) {
        if (*p == '"') { *o++ = '"'; *o++ = '"'; }
        else *o++ = *p;
    }
    *o++ = '"'; *o = '\0';
    return out;
}
