#pragma once

#include "ansi.h"

static const char* zno_help_text = 
    "Usage: " ANSI_COLOR(YELLOW, "zno") " [options]                                         \n"
    "       " ANSI_COLOR(YELLOW, "zno") " <command> [<args>]                                \n"
    "                                                                                       \n"
    "Options:                                                                               \n"
    "   -v, --version = Display version information. Does not create note.                  \n"
    "   -h, --help    = Display this help text. Does not create note.                       \n"
    "                                                                                       \n"
    "   -n <note>     = Create new note without opening it in a text editor.                \n"
    "   -t <tags>     = Add tags (comma-separated) to new note.                             \n"
    "   -T <title>    = Add title to new note.                                              \n"
    "                                                                                       \n"
    "   -s <term>     = Search for term within tags and note bodies.                        \n"
    "   -l [<tag>]    = List tags. If a tag is specified, list notes with                   \n"
    "                         that tag                                                      \n"
    "   -L [<tag>]    = List global lists from notes. If tag is specified,                  \n"
    "                         only show that specific global list.                          \n"
    "                                                                                       \n"
    "Available commands                                                                     \n"
    "   " ANSI_COLOR(YELLOW, "init") "        Create empty notebox                          \n"
    "   " ANSI_COLOR(YELLOW, "build") "       Build metadata pages and webpages             \n"
    "   " ANSI_COLOR(YELLOW, "help") "        Display this help text. Does not create note. \n"
    "                                                                                       \n"
    "See --help from sub-commands for more about that command                               \n";