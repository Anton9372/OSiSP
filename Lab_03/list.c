#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>


node* constructorList(pid_t pid) {
    node* val = (node*)malloc(1 * sizeof(node));
    val->next = NULL;
    val->pid = pid;
    return val;
}

void pushList(node** head, pid_t pid) {
    if (head == NULL) {
        exit(-1);
    }
    if (*head == NULL) {
        *head = constructorList(pid);
        return;
    }
    node* current = *head;
    while(current->next != NULL) {
        current = current->next;
    }
    current->next = constructorList(pid);
}

void displayList(const node* head) {
    if (head != NULL) {
        printf("Parent PID: %d\n", head->pid);
    } else {
        return;
    }
    if (head->next != NULL) {
        head = head->next;
        size_t i = 1;
        while(head != NULL) {
            printf("Child%lu PID: %d\n", i++, head->pid);
            head = head->next;
        }
    }
}

pid_t popList(node** head) {
    if (head == NULL) {
        exit(-1);
    }     
    if (*head == NULL) {
        return -1;
    }

    node* current = *head;
    node* prev = NULL;

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

void clearList(node** head) {
    if (head == NULL) {
        return;
    }

    node* current = *head;
    node* next;

    while (current != NULL) {
        next = current->next;
        free(current);
        current = next;
    }

    *head = NULL;
}
