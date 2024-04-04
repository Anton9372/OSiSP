#ifndef LIST_H
#define LIST_H

#include <sys/types.h>

typedef struct node {
    pid_t pid;
    struct node* next;
}node;

node* constructorList(pid_t pid);
void pushList(node** head, pid_t pid);
void displayList(const node* head);
pid_t popList(node** head);
void clearList(node** head);

#endif