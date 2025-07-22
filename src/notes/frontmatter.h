#pragma once

#include <ctype.h>

struct {
    const char* title;
    size_t num_tags;
    const char* tags[256];
    bool publish;
    size_t frontmatter_size;
} typedef frontmatter;

bool parse_frontmatter(char* frontmatter_buf, frontmatter* out);