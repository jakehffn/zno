
#include "init_command.h"

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