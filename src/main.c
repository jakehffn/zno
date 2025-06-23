#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_COLOR(color, text) ANSI_COLOR_##color text ANSI_COLOR_RESET

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
                printf(
                    "Usage: " ANSI_COLOR(YELLOW, "zno") " [options]                                 \n"
                    "       " ANSI_COLOR(YELLOW, "zno") " <command> [<args>]                        \n"
                    "                                                                               \n"
                    "Options:                                                                       \n"
                    "   -v, --version = Display version information. Does not create note.          \n"
                    "   -h, --help    = Display this help text. Does not create note.               \n"
                    "                                                                               \n"
                    "   -n <note>     = Create new note without opening it in a text editor.        \n"
                    "   -t <tags>     = Add tags (comma-separated) to new note.                     \n"
                    "   -T <title>    = Add title to new note.                                      \n"
                    "                                                                               \n"
                    "   -s <term>     = Search for term within tags and note bodies.                \n"
                    "   -l [<tag>]    = List tags. If a tag is specified, list notes with           \n"
                    "                         that tag                                              \n"
                    "   -L [<tag>]    = List global lists from notes. If tag is specified,          \n"
                    "                         only show that specific global list.                  \n"
                    "                                                                               \n"
                    "Available commands                                                             \n"
                    "   " ANSI_COLOR(YELLOW, "init") "        Create empty notebox                  \n"
                    "   " ANSI_COLOR(YELLOW, "build") "       Build metadata pages and webpages     \n"
                    "                                                                               \n"
                    "See --help from sub-commands for more about that command                       \n"
                );
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

    char* zno_editor = getenv("ZNO_EDITOR");
    if (zno_editor == NULL) {
        print_warning("env variable ZNO_EDITOR is not set\ndefaulting to \"code\"");
        zno_editor = "code";
    }

    if (should_create_note) {
        const char* path = create_note(note_content, note_title, note_tags);

        if (open_note_after_create && path) {
            char command_buf[PATH_MAX + 64];
            sprintf(command_buf, "%s %s\n", zno_editor, path);
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

    if (getcwd(path, sizeof(path)) != NULL) {
        strcat(strcat(path, "\\"), argv[1]);

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

int main(int argc, char* argv[]) {
    // Check for sub command
    if (argc > 1 && argv[1][0] != '-') {
        const char* sub_command = argv[1];

        // Ignore the first argument
        --argc;
        argv = &argv[1];

        if (strcmp(sub_command, "init") == 0) {
            init_command(argc, argv);
        } else if (strcmp(sub_command, "build") == 0) {
            build_command(argc, argv);
        } else {
            fprintf(stderr, "Unknown command -- %s\n", sub_command);
        }        
    } else {
        default_command(argc, argv);
    }
}