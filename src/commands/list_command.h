#pragma once 

#include "config.h"
#include "util.h"

struct {
    char (*items)[ZNO_FILENAME_LENGTH+1];
    size_t size;
    size_t max_size;
} typedef note_list;

void list_command(int argc, char* argv[]);

void list_last_notes();

bool get_recent(const char* path, DIR* dir, struct dirent* entry, void* user_data);