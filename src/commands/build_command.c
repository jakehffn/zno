#include "build_command.h"

void build_command(int argc, char* argv[]) {
    const char* zno_dir = getenv("ZNO_DIR");
    if (zno_dir == NULL) {
        print_error("env variable ZNO_DIR is not set");
        return;
    }

    printf("Building %s", zno_dir);

    char base_path[PATH_MAX+1] = "\0";
    char zno_path[PATH_MAX+1] = "\0";
    char tags_path[PATH_MAX+1] = "\0";
    char links_path[PATH_MAX+1] = "\0";
    char backlinks_path[PATH_MAX+1] = "\0";

    strcpy(base_path, zno_dir);
    strcpy(zno_path, zno_dir);
    strcat(zno_path, "/.zno");
    
    char* sub_path_bufs[] = { tags_path, links_path, backlinks_path };
    const char* sub_paths[] = { "/tags", "/links", "/backlinks" };
    for (size_t it = 0; it < sizeof(sub_path_bufs)/sizeof(sub_path_bufs[0]); ++it) {
        char* sub_path_buf = sub_path_bufs[it];
        const char* sub_path = sub_paths[it];

        strcpy(sub_path_buf, zno_path);
        strcat(sub_path_buf, sub_path);
        ensure_directory(sub_path_buf);
    }
    
    char* base_path_end = base_path + strlen(base_path);
    char* tags_path_end = tags_path + strlen(tags_path);
    char* links_path_end = links_path + strlen(links_path);
    char* backlinks_path_end = backlinks_path + strlen(backlinks_path);

    iterate_directory(base_path, it_func, NULL);
}

bool it_func(const char* path, DIR* dir, struct dirent* entry, void* usr_data) {
    printf("%s\n", entry->d_name);
}