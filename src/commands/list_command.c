#include "list_command.h"

void list_command(int argc, char* argv[]) {
    note_matching_command(argc, argv, list_help_text, matching_it_proc, NULL);
}

bool matching_it_proc(const char* note_name, frontmatter* fm, char* note_body, void* usr_data) {
    const char* zno_dir = getenv(ZNO_DIR);
    if (zno_dir == NULL) {
        print_error("env variable " ZNO_DIR " is not set");
        return false;
    }

    if (fm == NULL) {
        return true;
    }

    const size_t visible_tags_width = 25;
    const size_t visible_title_width = 40;

    printf(ANSI_BG(BRIGHT_BLACK, ".\\%*s - %s\\%s") "\n", 
        ZNO_FILENAME_LENGTH, note_name, zno_dir, note_name
    );

    // printf("    num_tags: %zu, publish: %s, frontmatter_size: %zu\n", fm.num_tags, (fm.publish) ? "TRUE" : "FALSE", fm.frontmatter_size);

    if (fm->title) {
        printf("  " ANSI_BG(BRIGHT_BLACK, "Title:") " %-*s\n", 
            visible_title_width, fm->title
        );
    }
    if (fm->num_tags > 0) {
        printf("  " ANSI_BG(BRIGHT_BLACK, " Tags:") " ", visible_tags_width);

        for (size_t it = 0; it < fm->num_tags; ++it) {
            printf(ANSI_COLOR(MAGENTA, "%s"), fm->tags[it]);
            if (it != fm->num_tags - 1) {
                printf(", ");
            }
        }
        printf("\n");
    }

    if (fm->publish) {
        printf("  " ANSI_BG(BRIGHT_BLACK, "      ") ANSI_COLOR(CYAN, " Published") "\n");
    }

    printf("\n");

    return true;
}