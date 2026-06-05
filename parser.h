#pragma once
#define MAX_ARGS 64
#define MAX_LINE 1024

// Parses a line of input into a NULL-terminated array
// Returns the number of tokens, or 0 if the line is empty.

typedef struct {
    char *argv[MAX_ARGS];
    int num_args;
    char *input_file;   // NULL if no redirection
    char *output_file;  // NULL if no redirection
    int append;         // 1 if >>, 0 if >
} Command;

typedef struct {
    Command commands[MAX_ARGS];
    int num_commands;
} Pipeline;

Pipeline parse_pipeline(char *line, int max_args);

