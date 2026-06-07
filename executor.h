#pragma once
#include "parser.h"
#include <sys/types.h>
#define MAX_JOBS 64


typedef enum {RUNNING, STOPPED, DONE} JobStatus;

typedef struct {
    int id;
    pid_t pgid;
    JobStatus status;
    char command[MAX_LINE];
} Job;

extern Job jobs[MAX_JOBS];
extern int num_jobs;


int execute_pipeline(Pipeline pipeline, char* line);
int execute_command(Command cmd);
int execute_builtin(char **argv);


