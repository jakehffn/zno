#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <dirent.h>

#include "util.h"
#include "config.h"

void default_command(int argc, char* argv[]);

const char* create_note(const char* content, const char* title, char* tags);