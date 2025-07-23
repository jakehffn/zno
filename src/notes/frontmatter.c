#include "frontmatter.h"

char* parse_frontmatter(char* frontmatter_buf, frontmatter* out) {
    if (!out || !frontmatter_buf) {
        return NULL;
    }

    const char* fm_delim = "---";
    const size_t fm_buf_size = strlen(frontmatter_buf);

    out->title = NULL;
    out->num_tags = 0;
    out->publish = false;

    char* tok = strtok(frontmatter_buf, " \t\n");
    // Check for start of frontmatter
    if (tok && strcmp(tok, fm_delim) != 0) {
        return NULL;
    }
    
    char* frontmatter_end = NULL;
    
    tok = strtok(NULL, " \t\n");
    while (tok) {
        char* tok_last = &tok[strlen(tok)-1];
        if (*tok_last == ':') {
            *tok_last = '\0';

            if (strcmp(tok, "title") == 0) {
                tok = strtok(NULL, "\n");
                out->title = trim(tok);
            }
            if (strcmp(tok, "tags") == 0) {
                tok = strtok(NULL, "\n");
                while (tok) {
                    char* it = tok;

                    // Get to first character in list element
                    while (*it && (*it == ' ' || *(it) == '\t')) ++it;
                    // If the element is not part of the list, break
                    // Not part of list if
                    //  - has no content
                    //  - same level as list name
                    //  - doesn't start with '-'
                    if (!(*it) || it == tok || *it != '-') break;
                    
                    out->tags[out->num_tags] = trim(++it);
                    ++out->num_tags;

                    tok = strtok(NULL, "\n");
                }
                if ((tok + strlen(tok)) - frontmatter_buf >= fm_buf_size) {
                    return NULL;
                }

                *(tok + strlen(tok)) = '\n';   
                tok = strtok(tok, " \t\n");
                continue;
            }
            if (strcmp(tok, "publish") == 0) {
                tok = strtok(NULL, " \t\n");
                for (char* it = tok; *it; ++it) *it = tolower(*it);

                out->publish = strcmp(tok, "true") == 0;
            }
        } else {
            if (strcmp(tok, fm_delim) == 0) {
                frontmatter_end = tok + 3;
                break;
            }
            return NULL;
        }
        tok = strtok(NULL, " \t\n");
    }

    out->frontmatter_size = frontmatter_end - frontmatter_buf;

    return frontmatter_end;
}