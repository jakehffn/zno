#include "zno.h"

void print_error(const char* fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    fprintf(stderr, ANSI_COLOR_RED "error: ");
    vfprintf(
        stderr, 
        fmt,
        argptr
    );
    fprintf(stderr, ANSI_COLOR_RESET "\n");
    va_end(argptr);
}

void print_warning(const char* fmt, ...) {
    va_list argptr;
    va_start(argptr, fmt);
    fprintf(stderr, ANSI_COLOR_YELLOW "warning: ");
    vfprintf(
        stderr, 
        fmt,
        argptr
    );
    fprintf(stderr, ANSI_COLOR_RESET "\n");
    va_end(argptr);
}

void replace_char(char* str, char find, char replace) {
    for (char* current_pos = str; (current_pos = strchr(str, find)) != NULL; *current_pos = replace);
}

bool ensure_directory(const char* path) {
    if (mkdir(path) == -1 && errno != EEXIST) {
        print_error("Failed to create directory %s\n    %s", path, strerror(errno));
        return false;
    }

    return true;
}

char* get_timestamp_str(void) {
    static char fmt[32]; 
    static char buf[32];
    struct timeval tv;
    struct tm* tm;

    mingw_gettimeofday(&tv, NULL);
    time_t t = tv.tv_sec;
    if((tm = gmtime(&t)) != NULL) {
        strftime(fmt, sizeof(fmt), "%Y-%m-%dT%H_%M_%S_%%06u", tm);
        snprintf(buf, sizeof(buf), fmt, tv.tv_usec);
    }

    return buf;
}

char* get_optional_arg(char* arg, int argc, char* argv[]) {
    // optional arg passed as -oarg for optional parameter for -o 'arg' will be in optarg
    //  otherwise, it needs to be manually captured
    if (arg != NULL) {
        return arg;
    }

    if (optind < argc && argv[optind][0] != '-') {
        arg = argv[optind];
        // Skip the optional argument
        ++optind;
    }
    
    return arg;
}

char* create_note(const char* content, const char* title, char* tags) {
    const char* zno_dir = getenv("ZNO_DIR");
    if (zno_dir == NULL) {
        print_error("env variable ZNO_DIR is not set");
        return NULL;
    }

    const char* file_name = get_timestamp_str();

    static char path[PATH_MAX+1];
    snprintf(path, PATH_MAX, "%s\\%s.md", zno_dir, file_name);

    FILE* fp = fopen(path, "a");

    if (fp == NULL) {
        print_error("Failed to open file %s\n    %s", path, strerror(errno));
        return NULL;
    }

    fprintf(fp, "---\n");

    if (title) {
        fprintf(fp, "title: %s\n", title);
    }

    fprintf(fp, "tags:\n");
    if (tags) {
        char* tag = tags;
        bool exit = false;
        for (char* c = tags;; ++c) {
            if ((*c == ',' || (exit = (*c == 0)))) {
                if (tag != c) {
                    // Trim tag begining
                    while (*tag == ' ') ++tag;
                    // Trim tag end
                    char* t = c;
                    while (*(--t) == ' ') *t = 0;
                    *c = 0;
                    fprintf(fp, "  - %s\n", tag);
                }
                tag = c+1;
            }
            
            if (exit) break;
        }
    }

    fprintf(fp, "---\n\n");

    if (title) {
        fprintf(fp, "# %s\n", title);
    }

    if (content) {
        fprintf(fp, content);
    }

    fclose(fp);

    return path;
}

void default_command(int argc, char* argv[]) {
    bool should_create_note = true;
    bool open_note_after_create = true;

    char* note_content = NULL;
    char* note_tags = NULL;
    char* note_title = NULL;

    char non_create_action = 0;
    char* non_create_arg = NULL;

    int opt;
    static struct option long_options[] = {
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {0, 0, 0, 0}
    };
    while ((opt = getopt_long(argc, argv, "vhn:t:T:s:l::L::c::", long_options, NULL)) != -1) {
        char* arg = optarg;
        switch (opt) {
            case 'v': {
                printf("v0.0.0\n");
                return;
            }
            // Help
            case 'h': {
                printf(zno_help_text);
                return;
            }
            // New
            case 'n': {
                note_content = strdup(optarg);
                open_note_after_create = false;
                break;
            }
            // Tags
            case 't': {
                note_tags = strdup(optarg);
                break;
            }
            // Title
            case 'T': {
                note_title = strdup(optarg);
                break;
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
                should_create_note = false;

                if (!non_create_action) {
                    non_create_action = opt;
                    non_create_arg = strdup(arg);
                }
                break;
            }
            default: break;
        }
    }

    const char* zno_dir = getenv("ZNO_DIR");
    if (zno_dir == NULL) {
        print_error("env variable ZNO_DIR is not set");
        return;
    }

    const char* zno_editor_cmd = getenv("ZNO_EDITOR_CMD");
    if (zno_editor_cmd == NULL) {
        print_warning("env variable ZNO_EDITOR_CMD is not set\ndefaulting to \"code\"");
        zno_editor_cmd = "code";
    }

    if (should_create_note) {
        const char* path = create_note(note_content, note_title, note_tags);

        if (open_note_after_create && path) {
            char command_buf[PATH_MAX + 128];

            strcpy(command_buf, zno_editor_cmd);
            const char* file_arg = "$FILE";
            char* file_arg_pos_buf = strstr(command_buf, file_arg);
            
            if (file_arg_pos_buf) {
                char* file_arg_pos_cmd = strstr(zno_editor_cmd, file_arg);
                while (file_arg_pos_cmd) {
                    strcpy(file_arg_pos_buf, path);
                    strcpy(file_arg_pos_buf + strlen(path), file_arg_pos_cmd + strlen(file_arg));
                    file_arg_pos_buf = strstr(command_buf, file_arg);
                    file_arg_pos_cmd = strstr(file_arg_pos_cmd + 1, file_arg);
                }
            } else {
                strcat(command_buf, " \"");
                strcat(command_buf, path);
                strcat(command_buf, "\"");
            }

            system(command_buf);
        }
    }

    switch(non_create_action) {
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
        default: break;
    }
    if (non_create_arg) printf("  With arg: %s", (non_create_arg));

    if (note_content)   free(note_content);
    if (note_tags)      free(note_tags);
    if (note_title)     free(note_title);
    if (non_create_arg) free(non_create_arg);
}

void init_command(int argc, char* argv[]) {
    char path[PATH_MAX+1] = "\0";
    char* base_path_end = path;

    // Use path if given, otherwise make relative to cwd
    if (argc > 1) {
        strcpy(path, argv[1]);
        strcat(path, "\\");
        base_path_end = path + strlen(path);
    } else {
        strcpy(path, ".");
    }

    // Init .zno folder and contents
    strcat(path, "/.zno");
    char* meta_path_end = path + strlen(path);
    ensure_directory(path);
    
    strcat(path, "/tags");
    ensure_directory(path);
    
    *meta_path_end = 0;
    strcat(path, "/links");
    ensure_directory(path);
    
    *meta_path_end = 0;
    strcat(path, "/backlinks");
    ensure_directory(path);

    // Create/append to .gitignore
    *base_path_end = 0;
    strncat(path, ".gitignore", PATH_MAX);
    FILE* fp = fopen(path, "a");
    if (fp != NULL) {
        fprintf(
            fp, 
            "\n# zno defaults\n"
            "/.zno\n"
            "/.build\n"
            "/.templates\n"
            "# end zno defaults\n"
        );
        fclose(fp);
    } else {
        print_error("Failed to open file %s\n    %s", path, strerror(errno));
    }
    
    bool can_display_absolute_path = (path[0] != '.' || path[0] != '/' || path[0] != '\\');
    if (can_display_absolute_path) {
        *(base_path_end-1) = 0;
    } else if (getcwd(path, sizeof(path)) != NULL) {
        strcat(strcat(path, "\\"), argv[1]);
        can_display_absolute_path = true;
    }

    if (can_display_absolute_path)  {
        printf("Initialized zettelkasten at %s\n\n", path);
        printf("Add the following to your env:\n    " ANSI_COLOR(YELLOW, "ZNO_DIR=\"%s\""), path);
    }
}

void build_command(int argc, char* argv[]) {
    const char* zno_dir = getenv("ZNO_DIR");
    if (zno_dir == NULL) {
        print_error("env variable ZNO_DIR is not set");
        return;
    }

    printf("Building %s", zno_dir);
}

void help_command(int argc, char* argv[]) {
    printf(zno_help_text);
}
