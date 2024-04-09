#include "list.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <stdbool.h>

nodeList* constructorList(pthread_t id, char role) {
    nodeList* val = (nodeList*)malloc(1 * sizeof(nodeList));
    val->next = NULL;
    val->pthreadId = id;
    val->pthreadRole = role;
    return val;
}

void pushList(nodeList** head, pthread_t id, char role) {
    if (head == NULL) {
        exit(-1);
    }
    if (*head == NULL) {
        *head = constructorList(id, role);
        return;
    }
    nodeList* current = *head;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = constructorList(id, role);
}

void displayList(const nodeList* head) {
    size_t i = 1;
    if (head == NULL) {
        printf("List is empty.\n");
        return;
    }
    while(head != NULL) {
        printf("Pthread_%lu_%c id = %lu\n", i++, head->pthreadRole, head->pthreadId);
        head = head->next;
    }
}

pthread_t popList(nodeList** head) {
    if (head == NULL) {
        exit(-1);
    }
    if (*head == NULL) {
        return -1;
    } 
    nodeList* current = *head;
    nodeList* prev = NULL;
    while(current->next != NULL) {
        prev = current;
        current = current->next;
    }
    pthread_t id = current->pthreadId;
    free(current);
    if (prev == NULL) {
        *head = NULL;
    } else {
        prev->next = NULL;
    }
    return id;
}

pthread_t eraseList(nodeList** head, size_t positionNum) {
    if (head == NULL) {
        exit(-2);
    }
    if (*head == NULL) {
        printf("List is empty.\n");
        return -1;
    }
    nodeList* prev = NULL;
    nodeList* current = *head;
    size_t i = 0;
    while((current->next != NULL) && (i != positionNum)) {
        prev = current;
        current = current->next;
        ++i;
    }
    if (i != positionNum) {
        printf("There is no element with this number.");
        return false;
    }
    if (prev == NULL) {
        *head = (*head)->next;  
    } else {
        prev->next = current->next;
    }
    pthread_t id = current->pthreadId;
    free(current);
    return id;
}

void clearList(nodeList** head) {
    if (head == NULL) {
        exit(-3);
    }
    if (*head == NULL) {
        printf("List is already empty.");
        return;
    }
    nodeList* current = *head;
    while (current != NULL) {
        nodeList* next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}