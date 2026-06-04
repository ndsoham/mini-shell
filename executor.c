#include "executor.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

int execute_command(Command cmd) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("Error, fork failed");
        return EXIT_FAILURE;
    } else if (pid == 0) {
        
        if (cmd.input_file) {
            int fd = open(cmd.input_file, O_RDONLY);

            if (dup2(fd, STDIN_FILENO) == -1) {
                perror("Dup2 failed for STDIN");
            }

            if (close(fd) == -1) {
                perror("Close failed for STDIN");
                return EXIT_FAILURE;
            }
        
        }

        if (cmd.output_file) {
            int fd;
            if (cmd.append) {
                fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {
                fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }

            if (dup2(fd, STDOUT_FILENO) == -1) {
                perror("Dup2 failed for STDOUT");
                return EXIT_FAILURE;
            }

            if (close(fd) == -1) {
                perror("Close failed for STDOUT");
                return EXIT_FAILURE;
            }
            
        }

        execvp(cmd.argv[0], cmd.argv);
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
    (void)argv;
    exit(EXIT_SUCCESS);
    return EXIT_SUCCESS;
}

static int builtin_pwd(char **argv) {
    (void)argv;
    char buf[PATH_MAX];

    if (getcwd(buf, PATH_MAX)==NULL) {
        perror("Get cwd failed");
        return EXIT_FAILURE;
    }
    printf("%s\n", buf);
    return EXIT_SUCCESS;

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