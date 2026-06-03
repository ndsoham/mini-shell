#include <stdio.h>
#include "parser.h"
#include "executor.h"
#include <stdlib.h>
#include <string.h>

int main() {
    FILE* input = stdin;
    char line[MAX_LINE];
    char* argv[MAX_ARGS];

    while(1) {
        printf("mysh> ");
        fflush(stdout);
        if (fgets(line, MAX_LINE, input) == NULL) break;
        parse_input(line, argv, MAX_ARGS);
        if(!execute_builtin(argv)) {
            execute_command(argv);
        }
    }

    return EXIT_SUCCESS;

}