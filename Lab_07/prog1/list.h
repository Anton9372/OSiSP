#ifndef LIST_H
#define LIST_H

#include <sys/types.h>
#include <pthread.h>
#include <stdbool.h>

typedef struct nodeList {
    char pthreadRole;
    pthread_t pthreadId;
    struct nodeList* next;
}nodeList;

nodeList* constructorList(pthread_t id, char role);
void pushList(nodeList** head, pthread_t id, char role);
void displayList(const nodeList* head);
pthread_t popList(nodeList** head);
pthread_t eraseList(nodeList** head, size_t positionNum);
void clearList(nodeList** head);                        

#endif