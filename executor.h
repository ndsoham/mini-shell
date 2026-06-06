#pragma once
#include "parser.h"


int execute_pipeline(Pipeline pipeline);
int execute_command(Command cmd);
int execute_builtin(char **argv);

typedef struct {
    pid_t pgid;
    char command[MAX_LINE];
    int status;
} Job;