#include "list_command.h"

void list_command(int argc, char* argv[]) {
    char action = 0;
    char* action_arg = NULL;

    int opt;
    static struct option long_options[] = {
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };
    while ((opt = getopt_long(argc, argv, "hs:l::L::c::", long_options, NULL)) != -1) {
        char* arg = optarg;
        switch (opt) {
            // Help
            case 'h': {
                printf(list_help_text);
                return;
            }
            // List checklists
            case 'l': 
            // List lists
            case 'L': 
            // Count
            case 'c': {
                arg = get_optional_arg(arg, argc, argv);
            }
            // Search
            case 's': {
                if (!action) {
                    action = opt;
                    action_arg = strdup(arg);
                }
                break;
            }
            default: break;
        }
    }

    switch(action) {
        case 's': {
            printf("Searching notes\n");
            break;
        }
        case 'l': {
            printf("Listing checklists\n");
            break;
        }
        case 'L': {
            printf("Listing lists\n");
            break;
        }
        case 'c': {
            printf("Counting tags\n");
            break;
        }
        default: {
            list_last_notes();
            break;
        }
    }
}

void list_last_notes() {
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
        const size_t visible_tags_width = 25;
        const size_t visible_title_width = 40;

        const char* file_name = list.items[it];
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

        frontmatter fm;
        if (parse_frontmatter(note_buf, &fm)) {
            printf(ANSI_BG(BRIGHT_BLACK, ".\\%*s - %s\\%s") "\n", 
                ZNO_FILENAME_LENGTH, list.items[it], zno_dir, list.items[it]
            );

            // printf("    num_tags: %zu, publish: %s, frontmatter_size: %zu\n", fm.num_tags, (fm.publish) ? "TRUE" : "FALSE", fm.frontmatter_size);
    
            if (fm.title) {
                printf(ANSI_BG(BRIGHT_BLACK, "    Title:") " %-*s\n", 
                    visible_title_width, fm.title
                );
            }
            if (fm.num_tags > 0) {
                printf(ANSI_BG(BRIGHT_BLACK, "     Tags:") " ", visible_tags_width);

                for (size_t it = 0; it < fm.num_tags; ++it) {
                    printf(ANSI_COLOR(MAGENTA, "%s"), fm.tags[it]);
                    if (it != fm.num_tags - 1) {
                        printf(", ");
                    }
                }
                printf("\n");
            }

            if (fm.publish) {
                printf(ANSI_BG(BRIGHT_BLACK, "          ") ANSI_COLOR(CYAN, " Published") "\n");
            }

            printf("\n");
        }
        

        free(note_buf);
    }

    if (max_notes > 0) {
        free(list.items);
    }
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