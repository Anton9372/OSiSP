#ifndef CHILD_LIB_H
#define CHILD_LIB_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void optionPlus(const char* fileName);
void optionStar(const char* fileName, char* envp[]);
void optionAmpersand(const char* fileName);

#endif



