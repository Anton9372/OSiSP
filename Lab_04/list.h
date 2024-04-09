#ifndef LIST_H
#define LIST_H

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct nodeList {
    char procRole;
    pid_t pid;
    struct nodeList* next;
}nodeList;

nodeList* constructorList(pid_t pid, char role);
void pushList(nodeList** head, pid_t pid, char role);
pid_t popList(nodeList** head);
pid_t eraseList(nodeList** head, size_t positionNum);
void displayList(const nodeList* head);
void clearList(nodeList** head);

#endif