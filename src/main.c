#include "zno.h"

int main(int argc, char* argv[]) {
    // Check for sub command
    if (argc > 1 && argv[1][0] != '-') {
        const char* sub_command = argv[1];

        // Ignore the first argument
        --argc;
        argv = &argv[1];

        bool command_found = false;
        for (size_t it = 0; it < sizeof(zno_commands)/sizeof(zno_command); ++it) {
            const zno_command* command = &zno_commands[it];
            if (strcmp(sub_command, command->name) == 0) {
                command->func(argc, argv);
                command_found = true;
                break;
            }
        }

        if (!command_found) {
            fprintf(stderr, "Unknown command -- %s\n\n", sub_command);
            fprintf(stderr, "Available commands: \n");

            for (size_t it = 0; it < sizeof(zno_commands)/sizeof(zno_command); ++it) {
                const zno_command* command = &zno_commands[it];
                fprintf(stderr, "    " ANSI_COLOR(YELLOW, "%s") "\n", command->name);
            }

            fprintf(stderr, "\n");
        }        
    } else {
        default_command(argc, argv);
    }
}