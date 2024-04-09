#ifndef RING_H
#define RING_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>


#define LEN_MESSAGE 255
#define OFFSET 4

typedef struct nodeRing {
    struct nodeRing* next;
    struct nodeRing* prev;
    u_int8_t message[LEN_MESSAGE];
    bool flagIsBusy;
}nodeRing;

typedef struct ringBuffer {
    size_t consumed;
    size_t produced;
    nodeRing* head;
    nodeRing* tail;
    size_t size;
}ringBuffer;

nodeRing* constructorNode();
ringBuffer* constructorBuffer();
bool ringIsEmpty(const ringBuffer* ring);
void appendRing(ringBuffer** ring, bool bufferIsWorkingNow);
int eraseRing(ringBuffer** ring);
void addMessage(ringBuffer* ring, const u_int8_t* message);
u_int8_t* extractMessage(ringBuffer* ring);
void displayRing(ringBuffer* ring);
void clearRing(ringBuffer* ring);


#endif
