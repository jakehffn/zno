#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <dirent.h>

#define ANSI_COLOR_RESET            "\x1b[39m"
#define ANSI_COLOR_RED              "\x1b[31m"
#define ANSI_COLOR_GREEN            "\x1b[32m"
#define ANSI_COLOR_YELLOW           "\x1b[33m"
#define ANSI_COLOR_BLUE             "\x1b[34m"
#define ANSI_COLOR_MAGENTA          "\x1b[35m"
#define ANSI_COLOR_CYAN             "\x1b[36m"
#define ANSI_COLOR_WHITE            "\x1b[37m"
#define ANSI_COLOR_BRIGHT_BLACK     "\x1b[90m"

#define ANSI_COLOR(color, text) ANSI_COLOR_##color text ANSI_COLOR_RESET

#define ANSI_BG_RESET               "\x1b[49m"
#define ANSI_BG_RED              "\x1b[41m"
#define ANSI_BG_GREEN            "\x1b[42m"
#define ANSI_BG_YELLOW           "\x1b[43m"
#define ANSI_BG_BLUE             "\x1b[44m"
#define ANSI_BG_MAGENTA          "\x1b[45m"
#define ANSI_BG_CYAN             "\x1b[46m"
#define ANSI_BG_WHITE            "\x1b[47m"
#define ANSI_BG_BRIGHT_BLACK     "\x1b[100m"

#define ANSI_BG(color, text) ANSI_BG_##color text ANSI_BG_RESET

#define TIMESTAMP_LENGTH 26
#define ZNO_FILENAME_LENGTH 29

void print_error(const char* fmt, ...);
void print_warning(const char* fmt, ...);
void replace_char(char* str, char find, char replace);
bool ensure_directory(const char* path);
void iterate_directory(const char* path, bool(*it_func)(const char* path, DIR* dir, struct dirent* entry, void* usr_data), void* user_data);

char* get_timestamp_str(void);
char* get_optional_arg(char* arg, int argc, char* argv[]);