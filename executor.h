#pragma once

int execute_command(char **argv);
int execute_builtin(char **argv);

static int builtin_cd(char **argv);
static int builtin_exit(char **argv);
static int builtin_pwd(char **argv);