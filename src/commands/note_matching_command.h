#pragma once

#include "config.h"
#include "util.h"
#include "frontmatter.h"

struct {
    char (*items)[ZNO_FILENAME_LENGTH+1];
    size_t size;
    size_t max_size;
} typedef note_list;


typedef bool(*matching_note_proc_t)(const char* note_name, frontmatter* fm, char* note_body, void* usr_data);
void note_matching_command(int argc, char* argv[], const char* help_text, matching_note_proc_t it_proc, void* usr_data);

void iterate_notes(const char* tag_filter_str, const char* title_filter_str, const char* publish_filter_str, matching_note_proc_t it_proc, void* usr_data);

bool read_note(const char* file_name, const char* tag_filter_str, const char* title_filter_str, const char* publish_filter_str, matching_note_proc_t it_proc, void* usr_data);

bool get_recent(const char* path, DIR* dir, struct dirent* entry, void* user_data);

bool tag_filter(char* filter, char* filter_end, size_t num_tags, const char* tags[]);