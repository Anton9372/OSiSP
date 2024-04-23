#include "lib.h"

int memsize;
int numOfBlocks;
int numOfThreads;

pthread_mutex_t mutex;
pthread_barrier_t barrier;

indexRecord* currentMemsizeBlock;


int main(int argc, char *argv[]) {

    if (argc != 5) {
        printf("Invalid num of arguments. 4 arguments expecting\n");
        exit(-1);
    }

    memsize = atoi(argv[MEMSIZE]);
    numOfBlocks = atoi(argv[BLOCKS]);
    numOfThreads = atoi(argv[THREADS]) - 1;
    
    if(memsize % getpagesize() != 0) {
        printf("Invalid memsize. Must be a multiple of 4096\n");
        exit(-1);
    }
    if(numOfThreads < 8 || numOfThreads > 8000) {
        printf("Invalid threads count. Must be from 8 to 8000\n");
        exit(-1);
    }
    if(numOfBlocks % 2 || numOfBlocks < numOfThreads) {
        printf("Invalid blocks count. Must be a multiple of 2 and more than threads count\n");
        exit(-1);
    }
    
    const char *fileName = argv[FILENAME];
    int blockSize = memsize / numOfBlocks;

    FILE *file = fopen(fileName, "rb+");
    if (file == NULL) {
        fprintf(stderr, "Open file error\n");
        exit(-2);
    }
    fseek(file, 0, SEEK_END);
    int fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);
    int fileDescriptor = fileno(file);


    pthread_mutex_init(&mutex, NULL);
    pthread_barrier_init(&barrier, NULL, numOfThreads);

    void* bufferBegin = mmap(NULL, fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, fileDescriptor, 0);

    bufferBegin += sizeof(uint64_t);

    currentMemsizeBlock = (indexRecord*)bufferBegin;

    pthread_t childPthreadIds[numOfThreads - 1];

    for (int i = 1; i < numOfThreads; i++) {
        threadArgs* args = (threadArgs*)malloc(sizeof(threadArgs));
        args->blockSize = blockSize;
        args->bufferBegin = (indexRecord*)bufferBegin;
        args->threadNum = i;
        pthread_create(&childPthreadIds[i - 1], NULL, sorting, args);
    }

    threadArgs* args = (threadArgs*)malloc(sizeof(threadArgs));
    args->blockSize = blockSize;
    args->bufferBegin = (indexRecord*)bufferBegin;
    args->threadNum = 0;

    sorting(args);

    for(int i = 1; i < numOfThreads; i++) {
        pthread_join(childPthreadIds[i - 1], NULL);
    }

    munmap(bufferBegin - sizeof(uint64_t), fileSize);

    pthread_barrier_destroy(&barrier);
    pthread_mutex_destroy(&mutex);
    
    fclose(file);

    return 0;
}