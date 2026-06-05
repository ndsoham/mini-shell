#include <stdio.h>
#include "parser.h"
#include "executor.h"
#include <stdlib.h>
#include <string.h>

int main() {
    FILE* input = stdin;
    char line[MAX_LINE];

    while(1) {
        printf("mysh> ");
        fflush(stdout);
        if (fgets(line, MAX_LINE, input) == NULL) break;
        Pipeline pipeline = parse_pipeline(line, MAX_ARGS);
        fflush(stdout);
        for (int i = 0; i < pipeline.num_commands; i++) {
            Command cmd = pipeline.commands[i];
            for (int j = 0; j < cmd.num_args; j++) {
                printf("%s\n", cmd.argv[j]);
                fflush(stdout);
            }
        }
        // Command cmd = parse_input(line, argv, MAX_ARGS);
        // if (cmd.num_args == 0) continue;
        // if(!execute_builtin(cmd.argv)) {
        //     execute_command(cmd);
        // }
    }

    return EXIT_SUCCESS;

}