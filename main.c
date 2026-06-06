#include <stdio.h>
#include "parser.h"
#include "executor.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

int main() {
    FILE* input = stdin;
    char line[MAX_LINE];

    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);

    while(1) {
        printf("mysh> ");
        fflush(stdout);
        if (fgets(line, MAX_LINE, input) == NULL) break;
        Pipeline pipeline = parse_pipeline(line, MAX_ARGS);
        if (pipeline.num_commands == 0) continue;
        if (pipeline.num_commands == 1 && execute_builtin(pipeline.commands[0].argv)) {
                continue;
        }
        execute_pipeline(pipeline);
    }

    return EXIT_SUCCESS;

}