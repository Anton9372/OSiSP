#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "ring.h"
#include "list.h"
#include <time.h>
#include <signal.h>

#define BUFFER_SIZE 6

enum messageStructure {
    TYPE = 0,
    HIGH_BYTE_HASH = 1,
    LOW_BYTE_HASH = 2,
    SIZE = 3,
    DATA_BEGIN = 4
};

sem_t* SEMAPHORE_EMPTY;
sem_t* SEMAPHORE_FILLED;
sem_t* SEMAPHORE_MUTEX;
bool continueFlag = true;

u_int16_t controlSum(const u_int8_t*, size_t);
void producer(int32_t);
void consumer(int32_t);
u_int8_t* generateMessage();
void handlerStopProc();
void displayMessage(const u_int8_t* message);
void deleteAllChildProcs(nodeList* head);

int main() {
    signal(SIGUSR1, handlerStopProc);

    sem_unlink("SEMAPHORE_FILLED");
    sem_unlink("SEMAPHORE_EMPTY");
    sem_unlink("SEMAPHORE_MUTEX");

    SEMAPHORE_FILLED = sem_open("SEMAPHORE_FILLED", O_CREAT, 0777, 0);
    SEMAPHORE_EMPTY = sem_open("SEMAPHORE_EMPTY", O_CREAT, 0777, BUFFER_SIZE);
    SEMAPHORE_MUTEX = sem_open("SEMAPHORE_MUTEX", O_CREAT, 0777, 1);

    ringSharedBuffer* ringQueue = NULL;
    nodeList* childProcsList = NULL;

    pushList(&childProcsList, getpid(), '-');

    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        append(&ringQueue);
    }

    printf("Shmid segment : %d\n", ringQueue->shmid);
    
    int status;

    do {
        char ch = getchar();
        switch(ch) {
            case 'p' : {
                pid_t pid = fork();
                if (pid == 0) { 
                    producer(ringQueue->shmid); 
                }
                else if (pid == -1) {
                    fprintf(stderr, "Failed to create child process.\n");
                    exit(-228);
                }
                else { 
                    pushList(&childProcsList, pid, 'P'); 
                }
                break;
            }
            case 'c' : {
                pid_t pid = fork();
                if (pid == 0) {  
                    consumer(ringQueue->shmid); 
                }
                else if (pid == -1) {
                    fprintf(stderr, "Failed to create child process.\n");
                    exit(-228);
                }
                else { 
                    pushList(&childProcsList, pid, 'C');   
                }
                break;
            }
            case 'l' : {
                displayList(childProcsList);
                break;
            }
            case 'r' : {
                displayRing(ringQueue);
                break;
            }
            case 'k' : {
                size_t num;
                scanf("%lu", &num);
                if (num == 0) { 
                    printf("This process is not a child process.\n"); 
                } else {
                    pid_t pid = eraseList(&childProcsList, num);
                    if (pid != -1) {
                        kill(pid, SIGUSR1);
                    }
                }
                break;
            }
            case 'q' : {
                deleteAllChildProcs(childProcsList);
                clearSharedMemory(ringQueue);
                continueFlag = false;
                break;
            }
            default : {
                printf("Incorrect input.\n");
                fflush(stdin); 
                break;
            }
        }
        waitpid(-1, &status, WNOHANG);
        getchar();
    }while(continueFlag);

    sem_unlink("SEMAPHORE_FILLED");
    sem_unlink("SEMAPHORE_EMPTY");
    sem_unlink("SEMAPHORE_MUTEX");

    sem_close(SEMAPHORE_MUTEX);
    sem_close(SEMAPHORE_EMPTY);
    sem_close(SEMAPHORE_FILLED);

    return 0;
}

void deleteAllChildProcs(nodeList* head) {
    while(head->next) {
        pid_t pid = popList(&head);
        kill(pid, SIGKILL);
    }
    printf("All child processes are deleted.\n");
}

u_int16_t controlSum(const u_int8_t* data, size_t length) {
    u_int16_t hash = 0;
    for (size_t i = 0; i < length; ++i) {
        hash += data[i];
    }
    return hash;
}

u_int8_t* generateMessage() {
    srand(time(NULL));
    u_int8_t* message = (u_int8_t*)calloc(LEN_MESSAGE , sizeof(u_int8_t));
    size_t size = 0;
    size_t data_size = 0;
    while(size == 0) {
        size = rand() % 257;
    }
    if (size == 256) {
        size = 0;
        data_size = 256;
    } else {
        data_size = ((size + 3) / 4) * 4;
    }
    for (size_t i = DATA_BEGIN; i < data_size; ++i) {
        message[i] = rand() % 256;
    }
    u_int16_t hash = controlSum(message, size);
    message[TYPE] = 1;
    message[HIGH_BYTE_HASH] = (hash >> 8) & 0xFF;
    message[LOW_BYTE_HASH] = hash & 0xFF;
    message[SIZE] = size;
    return message;
}

void displayMessage(const u_int8_t* message) {
    size_t messageSize = 0;
    if (message[SIZE] == 0) {
        messageSize = LEN_MESSAGE;
    } else {
        messageSize = message[SIZE] + OFFSET;
    }
    for (size_t i = 0; i < messageSize; ++i)
        printf("%02X", message[i]);
    printf("\n");
}

void handlerStopProc() {
    continueFlag = false;
}

void consumer(int32_t shmid) {
    ringSharedBuffer* queue = shmat(shmid, NULL, 0);
    do {
        sem_wait(SEMAPHORE_FILLED);
        sem_wait(SEMAPHORE_MUTEX);
        sleep(2);
        u_int8_t* message = extractMessage(queue);
        sem_post(SEMAPHORE_MUTEX);
        sem_post(SEMAPHORE_EMPTY);
        displayMessage(message);
        free(message);
        printf("Consumed from CHILD with PID = %d\n", getpid());
        printf("Total messages retrieved = %lu\n", queue->consumed);
    }while(continueFlag);
    shmdt(queue);
}

void producer(int32_t shmid) {
    ringSharedBuffer* queue = shmat(shmid, NULL, 0);
    do {
        sem_wait(SEMAPHORE_EMPTY);
        sem_wait(SEMAPHORE_MUTEX);
        sleep(2);
        u_int8_t* new_message = generateMessage();
        addMessage(queue, new_message);
        sem_post(SEMAPHORE_MUTEX);
        sem_post(SEMAPHORE_FILLED);
        free(new_message);
        printf("Produced from CHILD with PID = %d\n", getpid());
        printf("Total ojbects created = %lu\n", queue->produced);
    }while(continueFlag);
    shmdt(queue);
}