#include <stdio.h>
#include "parser.h"
#include "executor.h"
#include <stdlib.h>
#include <string.h>
#include <signal.h>

void sigchld_handler(int signo) {
    (void)signo;
    int status;

    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].status == RUNNING) {
            pid_t result = waitpid(-jobs[i].pgid, &status, WNOHANG | WUNTRACED);
            if (result > 0) {
                if (WIFEXITED(status) || WIFSIGNALED(status)) {
                    jobs[i].status = DONE;
                    printf("\n[%d] Done\t%s\n", jobs[i].id + 1, jobs[i].command);
                }
            }
        }
    }
}

int main() {
    FILE* input = stdin;
    char line[MAX_LINE];
    char line_copy[MAX_LINE];

    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTSTP, &sa, NULL);
    sigaction(SIGTTOU, &sa, NULL);

    struct sigaction sa_chld;
    sa_chld.sa_handler = sigchld_handler;
    sigemptyset(&sa_chld.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa_chld, NULL);

    while(1) {
        printf("mysh> ");
        fflush(stdout);
        if (fgets(line, MAX_LINE, input) == NULL) break;
        strncpy(line_copy, line, MAX_LINE);
        Pipeline pipeline = parse_pipeline(line, MAX_ARGS);
        if (pipeline.num_commands == 0) continue;
        if (pipeline.num_commands == 1 && execute_builtin(pipeline.commands[0].argv)) {
                continue;
        }
        execute_pipeline(pipeline, line);
    }

    return EXIT_SUCCESS;

}

