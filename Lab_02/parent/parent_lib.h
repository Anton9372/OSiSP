#ifndef PARENT_LIB_H
#define PARENT_LIB_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int strCompare(const void* a, const void* b);
ssize_t getNumberOfEnvironmentVariables(char* envp[]);
void printEnvironmentVariables(char* envp[]);
const char* findEnvpVariableValue(char* envp[], const char* nameOfVariable);
const char* findEnvironVariableValue(const char* nameOfVariable);
void createChildProcess(char* operation, char* filename, char* envp[], short* counter);

#endif
