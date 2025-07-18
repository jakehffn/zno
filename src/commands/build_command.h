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

void build_command(int argc, char* argv[]);

bool it_func(const char* path, DIR* dir, struct dirent* entry, void* usr_data);