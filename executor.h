#pragma once
#include "parser.h"


int execute_pipeline(Pipeline pipeline);
int execute_command(Command cmd);
int execute_builtin(char **argv);