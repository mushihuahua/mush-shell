#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <iterator>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>



void tokenize(std::vector<std::string> &args, std::vector<std::string> &piped_args, std::string command);

int execute_builtin(std::vector<std::string> &args, std::vector<std::string> &piped_args);

int execute_cmd(std::vector<std::string> str_args);

int execute_pipe_cmd(std::vector<std::string> str_args, std::vector<std::string> str_piped_args);