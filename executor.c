#include "executor.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#define READ 0
#define WRITE 1

Job jobs[MAX_JOBS];
int num_jobs = 0;

int execute_command(Command cmd) {
    
    if (cmd.input_file) {
        int fd = open(cmd.input_file, O_RDONLY);

        if (fd == -1) {
            perror("open failed for STDIN");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDIN_FILENO) == -1) {
            perror("Dup2 failed for STDIN");
            exit(EXIT_FAILURE);
        }

        if (close(fd) == -1) {
            perror("Close failed for STDIN");
            exit(EXIT_FAILURE);
        }
    
    }

    if (cmd.output_file) {
        int fd;
        if (cmd.append) {
            fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd = open(cmd.output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }

        if (fd == -1) {
            perror("Open failed for STDOUT");
            exit(EXIT_FAILURE);
        }

        if (dup2(fd, STDOUT_FILENO) == -1) {
            perror("Dup2 failed for STDOUT");
            exit(EXIT_FAILURE);
        }

        if (close(fd) == -1) {
            perror("Close failed for STDOUT");
            exit(EXIT_FAILURE);
        }
    }

    execvp(cmd.argv[0], cmd.argv);
    perror("Error, execvp failed");
    exit(EXIT_FAILURE);
    
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

static int builtin_jobs(char **argv) {
    (void)argv;
    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].status != DONE) {
            char *status_str = jobs[i].status == RUNNING ? "Running" : "Stopped";
            printf("[%d] %s\t%s\n", jobs[i].id + 1, status_str, jobs[i].command);
        }
    }
    return EXIT_SUCCESS;
}

static int builtin_fg(char **argv) {
    if (argv[1] == NULL) {
        fprintf(stderr, "fg: missing job id\n");
        return EXIT_FAILURE;
    }

    int job_id = atoi(argv[1] + 1) - 1;
    int found = 0;
    for (int i = 0; i < num_jobs; i++) {
        if (jobs[i].id == job_id && jobs[i].status == STOPPED) {
            found = 1;
            if(tcsetpgrp(STDIN_FILENO, jobs[i].pgid) == -1) {
                perror("tcsetpgrp failed.");
                return EXIT_FAILURE;
            }

            if (kill(-jobs[i].pgid, SIGCONT) == -1) {
                perror("kill failed.");
                return EXIT_FAILURE;
            }

            jobs[i].status = RUNNING;

            while(1) {
                int status;
                pid_t pid = waitpid(-jobs[i].pgid, &status, WUNTRACED);
                if (pid == -1) {
                    if (errno == ECHILD) break;
                    perror("waitpid failed");
                    break;
                }
                if (WIFSTOPPED(status)) {
                    jobs[i].status = STOPPED;
                    printf("\n[%d] Stopped\t%s\n", jobs[i].id + 1, jobs[i].command);
                    break;
                }
            }

            if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1) {
                perror("tcsetpgrp failed.");
                return EXIT_FAILURE;
            }

            break;
        }
    }

    if (!found) {
        fprintf(stderr, "fg: job %d not found\n", job_id + 1);
    }

    return EXIT_SUCCESS;

}

static int builtin_bg(char ** argv) {
    
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

    if (strcmp(argv[0], "jobs") == 0) {
        builtin_jobs(argv);
        return 1;
    }

    if (strcmp(argv[0], "fg") == 0) {
        builtin_fg(argv);
        return 1;
    }

    if (strcmp(argv[0], "bg") == 0) {
        builtin_bg(argv);
        return 1;
    }

    return 0;
}

int execute_pipeline(Pipeline pipeline, char* line) {
    
    int fds[pipeline.num_commands-1][2];
    pid_t pids[pipeline.num_commands];

    for (int i = 0; i < pipeline.num_commands - 1; i++) {
        if (pipe(fds[i]) == -1) {
            perror("Error, pipe failed.");
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < pipeline.num_commands; i++) {
        pid_t pid = fork();

        if (pid == -1) {
            perror("Error, fork failed.");
            return EXIT_FAILURE;
        } else if (pid == 0) {
            
            if (setpgid(0, i > 0 ? pids[0] : 0) == -1) {
                perror("Error, setpgid failed.");
                exit(EXIT_FAILURE);
            }

            if (i > 0) {
                pipeline.commands[i].input_file = NULL;
                dup2(fds[i-1][READ], STDIN_FILENO);
            }

            if (i < pipeline.num_commands - 1) {
                pipeline.commands[i].output_file = NULL;
                dup2(fds[i][WRITE], STDOUT_FILENO);
            }

            for (int j = 0; j < pipeline.num_commands - 1; j++) {
                if (close(fds[j][READ]) == -1 || close(fds[j][WRITE]) == -1){
                    perror("Error, close failed in child.");
                    exit(EXIT_FAILURE);
                }
            }

            struct sigaction sa;
            sa.sa_handler = SIG_DFL;
            sigemptyset(&sa.sa_mask);
            sa.sa_flags = 0;
            sigaction(SIGINT, &sa, NULL);
            sigaction(SIGTSTP, &sa, NULL);

            if (!execute_builtin(pipeline.commands[i].argv)) {
                execute_command(pipeline.commands[i]);
            }

            exit(EXIT_SUCCESS);

        } else {
            pids[i] = pid;

            if (setpgid(pid, i > 0 ? pids[0] : pid) == -1) {
                perror("Error, set pgid failed.");
                return EXIT_FAILURE;
            }

            if (!pipeline.bg && i == 0 && tcsetpgrp(STDIN_FILENO, pid) == -1) {
                perror("Error, tcsetgrp failed.");
                return EXIT_FAILURE;
            }

        }
    }

    for (int j = 0; j < pipeline.num_commands - 1; j++) {
        if (close(fds[j][READ]) == -1 || close(fds[j][WRITE]) == -1){
            perror("Error, close failed in parent.");
            return EXIT_FAILURE;
        }
    }

    if (pipeline.bg) {
        jobs[num_jobs].id = num_jobs;
        jobs[num_jobs].pgid = pids[0];
        jobs[num_jobs].status = RUNNING;
        strncpy(jobs[num_jobs].command, line, MAX_LINE);
        num_jobs++;
        printf("[%d] %d\n", num_jobs, pids[0]);
        return EXIT_SUCCESS;
    }

    for (int i = 0; i < pipeline.num_commands; i++) {
        int status;
        if (waitpid(pids[i], &status, WUNTRACED) == -1) {
            perror("Error, waitpid failed.");
            return EXIT_FAILURE;
        }
        if (WIFSTOPPED(status)) {
            jobs[num_jobs].id = num_jobs;
            jobs[num_jobs].pgid = pids[0];
            jobs[num_jobs].status = STOPPED;
            strncpy(jobs[num_jobs].command, line, MAX_LINE);
            num_jobs++;
            printf("\n[%d] Stopped\t%s\n", num_jobs, line);
            break;
        }
    }

    if (tcsetpgrp(STDIN_FILENO, getpgrp()) == -1) {
        perror("Error, tcsetpgrp failed.");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}