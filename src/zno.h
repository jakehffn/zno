#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

#include "help_text.h"
#include "ansi.h"

void print_error(const char* fmt, ...);
void print_warning(const char* fmt, ...);
void replace_char(char* str, char find, char replace);
bool ensure_directory(const char* path);

char* get_timestamp_str(void);
char* get_optional_arg(char* arg, int argc, char* argv[]);
char* create_note(const char* content, const char* title, char* tags);

void default_command(int argc, char* argv[]);
void init_command(int argc, char* argv[]);
void build_command(int argc, char* argv[]);
void help_command(int argc, char* argv[]);

struct {
    const char* name;
    void (*func)(int argc, char* argv[]);
} typedef zno_command;

static const zno_command zno_commands[] = {
    {"build", build_command},
    {"init", init_command},
    {"help", help_command}
};