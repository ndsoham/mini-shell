#include "parser.h"
#include <string.h>

int parse_input(char *line, char **argv, int max_args) {
    line[strcspn(line, "\n")] = '\0';
    const char* delims = " \t";
    char* pch = strtok(line, delims);
    int i = 0;
    while (pch != NULL && i < max_args - 1) {
        argv[i++] = pch;
        pch = strtok(NULL, delims);
    }
    argv[i] = NULL;

    return i;
}