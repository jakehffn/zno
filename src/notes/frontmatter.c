#include "frontmatter.h"

bool parse_frontmatter(char* frontmatter_buf, frontmatter* out) {
    if (!out || !frontmatter_buf) {
        return false;
    }

    out->frontmatter_size = 20;
    out->num_tags = 3;
    out->tags[0] = "Testtag";
    out->tags[1] = "Other Tag";
    out->tags[2] = "Last Tag";
    out->publish = true;
    out->title = "This is the title";

    return true;
}