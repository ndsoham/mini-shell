#include "executor.h"
#include "unistd.h"
#include "sys/wait.h"
#include "stdio.h"
#include "stdlib.h"

int execute_command(char** argv) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error, fork failed");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        execvp(argv[0], argv);
        perror("Error, execvp failed");
        exit(EXIT_FAILURE);
    } else {
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid failed");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
    
}