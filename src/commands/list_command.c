#include "list_command.h"

void list_command(int argc, char* argv[]) {
    bool should_create_note = true;
    bool open_note_after_create = true;

    char* note_content = NULL;
    char* note_tags = NULL;
    char* note_title = NULL;

    char non_create_action = 0;
    char* non_create_arg = NULL;

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
}