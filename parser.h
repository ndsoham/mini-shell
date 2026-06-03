#pragma once
#define MAX_ARGS 64
#define MAX_LINE 1024

// Parses a line of input into a NULL-terminated array
// Returns the number of tokens, or 0 if the line is empty.

int parse_input(char *line, char **argv, int max_args);