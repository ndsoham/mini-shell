#include <stdio.h>
#include "parser.h"
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
        int num_tokens = parse_input(line, argv, MAX_ARGS);

        for(int i = 0; i < num_tokens; i++) {
            printf("[%d]: %s\n", i, argv[i]);
        }

    }

    return EXIT_SUCCESS;

}