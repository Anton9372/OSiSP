#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

nodeList* constructorList(pid_t pid, char role) {
    nodeList* val = (nodeList*)malloc(1 * sizeof(nodeList));
    val->next = NULL;
    val->pid = pid;
    val->procRole = role;
    return val;
}

void pushList(nodeList** head, pid_t pid, char role) {
    if (head == NULL) {
        exit(-1);
    }
    if (*head == NULL) {
        *head = constructorList(pid, role);
        return;
    }
    nodeList* current = *head;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = constructorList(pid, role);
}

void displayList(const nodeList* head) {
    if (head != NULL) {
        printf("Parent PID: %d\n", head->pid);
    } else {
        return;
    }
    if (head->next != NULL) {
        head = head->next;
        size_t i = 1;
        while(head != NULL) {
            printf("Child_%lu_%c with PID: %d\n", i++, head->procRole, head->pid);
            head = head->next;
        }
    }
}

pid_t popList(nodeList** head) {
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
    pid_t pid = current->pid;
    free(current);
    if (prev == NULL) {
        *head = NULL;
    } else {
        prev->next = NULL;
    }
    return pid;
}

pid_t eraseList(nodeList** head, size_t positionNum) {
    if (head == NULL) {
        exit(-2);
    }
    if (*head == NULL) {
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
        return -1;
    }
    pid_t pid = current->pid;
    if (prev == NULL) {
        *head = (*head)->next;
    } else {
        prev->next = current->next;
    }
    free(current);
    return pid;
}

void clearList(nodeList** head) {
    if (head == NULL) {
        exit(-3);
    }
    nodeList* current = *head;
    while (current != NULL) {
        nodeList* next = current->next;
        free(current);
        current = next;
    }
    *head = NULL;
}