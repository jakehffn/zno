#include "default_command.h"

char* create_note(const char* content, const char* title, char* tags) {
    const char* zno_dir = getenv(ZNO_DIR);
    if (zno_dir == NULL) {
        print_error("env variable " ZNO_DIR " is not set");
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
            default: break;
        }
    }

    const char* zno_dir = getenv(ZNO_DIR);
    if (zno_dir == NULL) {
        print_error("env variable " ZNO_DIR " is not set");
        return;
    }

    const char* zno_editor_cmd = getenv(ZNO_EDITOR_CMD);
    if (zno_editor_cmd == NULL) {
        print_warning("env variable " ZNO_EDITOR_CMD " is not set\ndefaulting to \"code\"");
        zno_editor_cmd = "code";
    }

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

    if (note_content)   free(note_content);
    if (note_tags)      free(note_tags);
    if (note_title)     free(note_title);
    if (non_create_arg) free(non_create_arg);
}
