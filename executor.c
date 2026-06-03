#include "executor.h"
#include "unistd.h"
#include "sys/wait.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

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

int execute_builtin(char** argv) {
    if (strcmp(argv[0], "cd") == 0) {
        builtin_cd(argv);
        return 1;
    }

    if (strcmp(argv[0], "exit") == 0) {
        builtin_exit(argv);
        return 1;
    }

    if (strcmp(argv[0], "pwd") == 0) {
        builtin_pwd(argv);
        return 1;
    }

    return 0;
}

static int builtin_cd(char **argv) {
    char* path = argv[1];

    if (path == NULL) {
        path = getenv("HOME");
    }

    if (chdir(path) == -1) {
        perror("Change directory failed");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

static int builtin_exit(char **argv) {
    return -1;
}

static int builtin_pwd(char **argv) {
    return -1;
}