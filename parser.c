#include "parser.h"
#include <string.h>
#include <stdio.h>
Command parse_input(char *line, char **argv, int max_args) {
    
    line[strcspn(line, "\n")] = '\0';

    Command cmd;
    cmd.input_file = NULL;
    cmd.output_file = NULL;
    cmd.append = 0;

    const char* delims = " \t";
    char* pch = strtok(line, delims);
    int i = 0;
    int input_idx=-1;
    int output_idx=-1;

    while (pch != NULL && i < max_args - 1) {
        argv[i++] = pch;
        pch = strtok(NULL, delims);
        
        
        if (pch && strcmp(pch, ">")==0){
            output_idx = i + 1;
        }

        if (pch && strcmp(pch, ">>")==0) {
            output_idx = i + 1;
            cmd.append = 1;
        }

        if (pch && strcmp(pch, "<")==0) {
            input_idx = i + 1;
        }
    }
    
    argv[i] = NULL;
    cmd.argv = argv;
    cmd.num_args = i;

    if (output_idx > 0 && output_idx < i) {
        cmd.argv[output_idx-1] = NULL;
        cmd.output_file = argv[output_idx];
    }

    if (input_idx > 0 && input_idx < i) {
        cmd.argv[input_idx-1] = NULL;
        cmd.input_file = argv[input_idx];
    }
    
    return cmd;
}