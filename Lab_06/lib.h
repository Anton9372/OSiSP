#ifndef LIB_H
#define LIB_H

#include <sys/types.h>
#include <pthread.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <sys/mman.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    double timeMark;
    uint64_t recno;
} indexRecord;

typedef struct {
    uint64_t numOfRecords;
    indexRecord* recordsArray;
} indexHeader;

enum cmdArgs {
    MEMSIZE = 1,
    BLOCKS = 2,
    THREADS = 3,
    FILENAME = 4
};

typedef struct {
    indexRecord* bufferBegin;
    int blockSize;
    int threadNum;
} threadArgs;   


extern int memsize;
extern int numOfBlocks;
extern int numOfThreads;

extern pthread_barrier_t barrier;
extern pthread_mutex_t mutex;
extern indexRecord* currentMemsizeBlock;

int compare(const void *a, const void *b);
void* sorting(void* threadArgs_);
void merge(void* threadArgs_);
void twoBlocksMerge(indexRecord* firstBlockBegin, int sizeOfOneBlockToMerge);

#endif