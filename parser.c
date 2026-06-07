#include "parser.h"
#include <string.h>
#include <stdio.h>

static Command parse_single_command(char *line, int max_args) {

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

        cmd.argv[i++] = pch;
        pch = strtok(NULL, delims);
    }
    
    cmd.argv[i] = NULL;
    cmd.num_args = i;

    return cmd;
}

Pipeline parse_pipeline(char *line, int max_args) {
    line[strcspn(line, "\n")] = '\0';

    Pipeline pipeline;
    pipeline.num_commands = 0;
    pipeline.bg = 0;

    char *amp = strrchr(line, '&');
    if (amp != NULL) {
        pipeline.bg = 1;
        *amp = '\0';
    }

    const char* pipe_char = "|";
    char* save_ptr;
    char *pseg = strtok_r(line, pipe_char, &save_ptr);
    int i = 0;

    while (pseg != NULL && i < max_args - 1) {
        pipeline.commands[i++] = parse_single_command(pseg, max_args);
        pseg = strtok_r(NULL, pipe_char, &save_ptr);
    }
    pipeline.num_commands = i;

    return pipeline;
}

