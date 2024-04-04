#ifndef RING_H
#define RING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/shm.h>
#include <string.h>

#define LEN_MESSAGE 255
#define OFFSET 4

typedef struct nodeRing {
    int32_t shmidCurr;
    struct nodeRing* next;
    struct nodeRing* prev;
    u_int8_t message[LEN_MESSAGE];
    bool flagIsBusy;
}nodeRing;

typedef struct ringSharedBuffer {
    int32_t shmid;
    size_t consumed;
    size_t produced;
    nodeRing* head;
    nodeRing* tail;
}ringSharedBuffer;

nodeRing* constructorNode();
ringSharedBuffer* constructorBuffer();
void append(ringSharedBuffer** ring);
void addMessage(ringSharedBuffer* ring, const u_int8_t* message);
u_int8_t* extractMessage(ringSharedBuffer* ring);
void displayRing(ringSharedBuffer* ring);
void clearSharedMemory(ringSharedBuffer* ring);


#endif
