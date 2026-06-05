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
        if (pipeline.num_commands == 0) continue;
        execute_pipeline(pipeline);
        
        // if(!execute_builtin(cmd.argv)) {
        //     execute_command(cmd);
        // }
    }

    return EXIT_SUCCESS;

}