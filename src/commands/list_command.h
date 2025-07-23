#pragma once 

#include "config.h"
#include "util.h"
#include "frontmatter.h"
#include "note_matching_command.h"

void list_command(int argc, char* argv[]);

bool matching_it_proc(const char* note_name, frontmatter* fm, char* note_body, void* usr_data);