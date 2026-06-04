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

    while (pch != NULL && i < max_args - 1) {
        
        if (strcmp(pch, ">")==0){
            pch = strtok(NULL, delims);
            if (pch == NULL) {
                fprintf(stderr, "mysh: syntax error: expected filename after >\n");
                cmd.num_args = 0;
                return cmd;
            }
            cmd.output_file = pch;
            pch = strtok(NULL, delims);
            continue;
        }

        if (strcmp(pch, ">>")==0) {
            pch = strtok(NULL, delims);
            if (pch == NULL) {
                fprintf(stderr, "mysh: syntax error: expected filename after >>\n");
                cmd.num_args = 0;
                return cmd;
            }
            cmd.output_file = pch;
            pch = strtok(NULL, delims);
            cmd.append = 1;
            continue;
        }

        if (strcmp(pch, "<")==0) {
            pch = strtok(NULL, delims);
            if (pch == NULL) {
                fprintf(stderr, "mysh: syntax error: expected filename after <\n");
                cmd.num_args = 0;
                return cmd;
            }
            cmd.input_file = pch;
            pch = strtok(NULL, delims);
            continue;
        }

        argv[i++] = pch;
        pch = strtok(NULL, delims);
    }
    
    argv[i] = NULL;
    cmd.argv = argv;
    cmd.num_args = i;
    
    return cmd;
}