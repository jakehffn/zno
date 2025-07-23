#pragma once

#include <stdlib.h>
#include <ctype.h>

#include <string.h>

#include "util.h"

struct {
    const char* title;
    size_t num_tags;
    const char* tags[256];
    bool publish;
    size_t frontmatter_size;
} typedef frontmatter;

char* parse_frontmatter(char* frontmatter_buf, frontmatter* out);