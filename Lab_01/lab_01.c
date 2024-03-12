#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include "string.h"
#include "unistd.h"

const int LEN = 255;

void intro()
{
    printf("%s\n", " |");
    printf("%s\n", " | Dirwalk: recurse into directory(s)");
    printf("%s\n", " |");
    printf("%s\n", " | Usage:");
    printf("%s\n", " |   ./study/other/dirwalk ./your/directory -[args]");
    printf("%s\n", " | [args]:");
    printf("%s\n", " |   -l, show symbolic links");
    printf("%s\n", " |   -d, show directories");
    printf("%s\n", " |   -f, show files");
    printf("%s\n", " |   -s, sort information");
    printf("%s\n", " |");
}

void outAndFree(char** info, int numOfStrings)
{
    for(int i = 0; i < numOfStrings; i++) {
        if(printf("%s\n", info[i]) < 0) {
            fprintf(stderr, "Error printing string: %s\n", strerror(errno));
            break;
        }
    }   
    for(int i = 0; i < numOfStrings; i++) {
        free(info[i]);
    }
    free(info);
}

void dirwalk(const char* dirName, bool l, bool d, bool f, char*** info, int* numOfStrings)
{
    DIR *dir = opendir(dirName);
    if(dir == NULL) {
        fprintf(stderr, "Directory opening error in %s: %s\n", dirName, strerror(errno)); 
        return;
    }
    struct dirent* dirEntry;
    while((dirEntry = readdir(dir)) != NULL) {
        if(strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) {
            continue;   
        }
        char* temp = (char*)calloc(LEN, sizeof(char));
        if(temp == NULL) {
            fprintf(stderr, "Memory allocation error: %s\n", strerror(errno)); 
            return;
        }
        if(snprintf(temp, LEN, "%s/%s", dirName, dirEntry->d_name) >= LEN) {
            fprintf(stderr, "Path is too long: %s%s\n", dirName, dirEntry->d_name);
        }
        if((dirEntry->d_type == DT_LNK && l) || (dirEntry->d_type == DT_DIR && d) || (dirEntry->d_type == DT_REG && f)) {
            (*numOfStrings)++;
            *info = (char**)realloc(*info, (*numOfStrings)*sizeof(char*));  
            if(*info == NULL) {
                fprintf(stderr, "Memory allocation error: %s\n", strerror(errno)); 
                return;
            }
            *((*info) + (*numOfStrings) - 1) = temp;
        }
        if(dirEntry->d_type == DT_DIR) {
            dirwalk(temp, l, d, f, info, numOfStrings);   
        }
    }
    closedir(dir);
}

int swapStr(char** str1, char** str2)
{
    if(*str1 == NULL || *str2 == NULL) {
        fprintf(stderr, "Swap strings error(NULL string): %s\n", strerror(errno)); 
        return 0;
    }
    char* temp = (char*)calloc(LEN, sizeof(char));
    if(temp == NULL) {
        fprintf(stderr, "Memory allocation error: %s\n", strerror(errno)); 
        return 0;
    }
    strcpy(temp, *str1);
    strcpy(*str1, *str2);
    strcpy(*str2, temp);
    free(temp);
    return 1;
}

void qSortStrs(char*** info, int left, int right)
{
    int i = left;
    int j = right;
    char* mid = *((*info) + (left + right) / 2);
    while(i < j) {
        while(strcmp(*((*info) + i), mid) < 0) {
            i++;
        }
        while(strcmp(*((*info) + j), mid) > 0) {
            j--;
        }
        if(i <= j) {
            if(swapStr((*info) + i, *info + j) == 0) {
                return;
            }
            i++;
            j--;
        }
    }
    if(left < j) {
        qSortStrs(info, left, j);
    }
    if(i < right) {
        qSortStrs(info, i, right);
    }
}

void sortStrs(char*** info, int numOfStrings)
{
    if(numOfStrings < 1 || *info == NULL) {
        fprintf(stderr, "Sort error, empty strings: %s\n", strerror(errno)); 
        return;
    }
    qSortStrs(info, 0, numOfStrings - 1);
}

int main(int argc, char** argv) {
    intro();
    int numOfStrings = 0;
    char** info = NULL;
    bool l = false, d = false, f = false, s = false;
    char* dirName = ".";
    int opt;
    while ((opt = getopt(argc, argv, "ldfs")) != -1) {
        switch (opt) {
            case 'l':
                l = true;
                break;
            case 'd':
                d = true;
                break;
            case 'f':
                f = true;
                break;
            case 's':
                s = true;
                break;
            default:
                fprintf(stderr, "Usage: %s [dir] [-ldfs]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    if (optind < argc) {
        dirName = argv[optind];
    }
    if(!l && !d && !f && !s)
        l = d = f = s = true;
    dirwalk(dirName, l, d, f, &info, &numOfStrings);
    if(s == true) {
        sortStrs(&info, numOfStrings);
    }
    outAndFree(info, numOfStrings);
    return 1;
}
