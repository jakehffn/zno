#include "note_matching_command.h"

void note_matching_command(int argc, char* argv[], const char* help_text, matching_note_proc_t it_proc, void* usr_data) {
    char action = 0;

    char* tag_filter_str = NULL;
    char* title_filter_str = NULL;
    char* publish_filter_str = NULL;

    int opt;
    static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {"tag", required_argument, NULL, 't'},
        {0, 0, 0, 0}
    };
    while ((opt = getopt_long(argc, argv, "ht:", long_options, NULL)) != -1) {
        char* arg = optarg;
        switch (opt) {
            // Help
            case 'h': {
                printf(help_text);
                return;
            }
            case 't': {
                tag_filter_str = strdup(optarg);
                break;
            }
            default: break;
        }
    }

    switch(action) {
        default: {
            iterate_notes(tag_filter_str, title_filter_str, publish_filter_str, it_proc, usr_data);
            break;
        }
    }

    if (tag_filter_str) free(tag_filter_str);
    if (title_filter_str) free(title_filter_str);
    if (publish_filter_str) free(publish_filter_str);
}


void iterate_notes(const char* tag_filter_str, const char* title_filter_str, const char* publish_filter_str, matching_note_proc_t it_proc, void* usr_data) {
    const char* zno_dir = getenv(ZNO_DIR);
    if (zno_dir == NULL) {
        print_error("env variable " ZNO_DIR " is not set");
        return;
    }

    size_t max_notes = 10;
    size_t list_size = sizeof(char)*(ZNO_FILENAME_LENGTH+1) * max_notes;
    note_list list = {
        malloc(list_size),
        0,
        max_notes
    };
    memset(list.items, 0, list_size);

    iterate_directory(zno_dir, get_recent, &list);

    for (size_t it =  0; it < list.size; ++it) {        
        if (!read_note(list.items[it], tag_filter_str, title_filter_str, publish_filter_str, it_proc, usr_data)) break;
    }

    if (max_notes > 0) {
        free(list.items);
    }
}

bool read_note(const char* file_name, const char* tag_filter_str, const char* title_filter_str, const char* publish_filter_str, matching_note_proc_t it_proc, void* usr_data) {
    const char* zno_dir = getenv(ZNO_DIR);
    if (zno_dir == NULL) {
        print_error("env variable " ZNO_DIR " is not set");
        return false;
    }

    static char file_path[PATH_MAX+1];
    snprintf(file_path, PATH_MAX, "%s\\%s", zno_dir, file_name);

    char* note_buf = NULL;
    FILE* fp = fopen(file_path, "r");
    if (fp != NULL) {
        if (fseek(fp, 0L, SEEK_END) == 0) {
            // Get length of file
            long bufsize = ftell(fp);

            note_buf = malloc(sizeof(char) * (bufsize + 1));

            fseek(fp, 0L, SEEK_SET);

            size_t new_length = fread(note_buf, sizeof(char), bufsize, fp);
            if (ferror(fp) != 0) {
                print_error("Failed to read file %s", file_path);
            } else {
                note_buf[new_length++] = '\0';
            }
        }
        fclose(fp);
    }



    bool result = true;
    frontmatter fm;
    char buf[256];
    strcpy(buf, "sony | (youtube & ideas)");
    if (parse_frontmatter(note_buf, &fm) 
        && tag_filter(buf, buf + strlen(buf), fm.num_tags, fm.tags)
    ) {
        result = it_proc(file_name, &fm, note_buf, usr_data);
    }
    
    free(note_buf);

    return result;
}

bool get_recent(const char* path, DIR* dir, struct dirent* entry, void* user_data) {
    char tmp_buf[ZNO_FILENAME_LENGTH+1];

    note_list* list = (note_list*) user_data;

    size_t insert_pos = 0;
    while (insert_pos < list->size && insert_pos < list->max_size && strcmp(entry->d_name, list->items[insert_pos]) <= 0) {
        ++insert_pos;
    }

    if (insert_pos < list->max_size) {
        size_t reverse_it = list->max_size - 1;

        while (reverse_it > insert_pos) {
            strcpy(list->items[reverse_it], list->items[reverse_it - 1]);
            --reverse_it;
        }
        strcpy(list->items[insert_pos], entry->d_name);
        list->size = (list->size + 1 > list->max_size) ? list->max_size : list->size + 1;
    }

    return true;
}

bool tag_filter(char* filter, char* filter_end, size_t num_tags, const char* tags[]) {
    if (filter >= filter_end) {
        return false;
    }

    char* it = filter;
    int p_sum = 0;
    char* p_start = NULL;
    char* p_end = NULL;
    for (; it != filter_end && (strchr("&|", *it) == NULL || p_sum != 0); it++) {
        if (*it == '(') {
            p_start = p_start ? p_start : it;
            ++p_sum;
        } else if (*it == ')') {
            --p_sum;
            p_end = (p_sum != 0 || p_end) ? p_end : it;
        }
    }

    if (it == filter_end && !p_start && !p_end) {
        filter = trim(filter);

        bool contains_check = false;

        switch(filter[0]) {
            // Escape character
            case '\\': {
                if (filter_end - filter == 1) {
                    return false;
                }
                // Escaped character
                switch (filter[1]) {
                    case '\\':
                        filter = &filter[1];
                        break;
                    case '?':
                        filter = &filter[1];
                        break;
                    // Not valid escape
                    default: 
                        return false;
                }
                break;
            }
            case '?': {
                contains_check = true;
                filter = &filter[1];
                break;
            }
            default: break;
        }

        if (contains_check) {
            for (size_t it = 0; it < num_tags; ++it) {
                if (strstr(tags[it], filter) != NULL) {
                    return true;
                }
            }
        } else {
            for (size_t it = 0; it < num_tags; ++it) {
                if (strcmp(tags[it], filter) == 0) {
                    return true;
                }
            }
        }
        return false;
    }
    char* left_end = it;
    if (p_start) {
        if (!p_end) return false;
        filter = p_start + 1;
        left_end = p_end - 1; 
        *p_end = '\0';
    }
    
    char it_val = *it;
    *it = '\0';

    if (it == filter_end) {\
        return tag_filter(filter, left_end, num_tags, tags);
    }
    
    switch (it_val) {
        case '&':
            return tag_filter(filter, left_end, num_tags, tags)
                && tag_filter(it + 1, filter_end, num_tags, tags);
        case '|':
            return tag_filter(filter, left_end, num_tags, tags)
                || tag_filter(it + 1, filter_end, num_tags, tags);
        default:
            return false;
    }

    return false;
}