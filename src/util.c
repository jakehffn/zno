#include "util.h"


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

void iterate_directory(const char* path, bool(*it_func)(const char* path, DIR* dir, struct dirent* entry, void* user_data), void* user_data) {
    DIR* dir = opendir(path);
    if (dir && it_func) {
        struct dirent* entry = NULL;
        while(dir) {
            if ((entry = readdir(dir)) != NULL) {
                if (!it_func(path, dir, entry, user_data)) break;
            } else if (errno == 0) {
                break;
            } else {
                print_error("Failed to read directory\n    %s", strerror(errno));
                break;
            }
        }

        closedir(dir);
    } else if (errno == ENOENT) {
        print_error("%s is not a directory", path);
    } else {
        print_error("Failed to read directory\n    %s", strerror(errno));
    }
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

char* trim(char* str) {
    if (str == NULL) {
        return NULL;
    }

    char* start = str;
    while (*start && *start == ' ' || *(start) == '\t') ++start;
    char* it = start + strlen(start);
    while (it != start && (*it == 0 || *it == ' ' || *it == '\t')) --it;
    *(it + 1) = 0;

    return start;
}