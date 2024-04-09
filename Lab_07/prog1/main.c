#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <time.h>
#include <stdbool.h>
#include "ring.h"
#include <pthread.h>
#include "list.h"
#include <signal.h>

#define BUFFER_SIZE 3

enum messageStructure {
    TYPE = 0,
    HIGH_BYTE_HASH = 1,
    LOW_BYTE_HASH = 2,
    SIZE = 3,
    DATA_BEGIN = 4
};

pthread_cond_t cond;
pthread_mutex_t mutex;

ringBuffer* ringQueue = NULL;

_Thread_local bool continueFlag = true;
bool breakProcessFlag = false;
size_t producerPasses = 0;

size_t CNT_FREE_PLACES = BUFFER_SIZE;
size_t CNT_FILL_PLACES = 0;

u_int16_t controlSum(const u_int8_t*, size_t);
void producer();
void consumer();
u_int8_t* generateMessage();
void handlerStopProc();
void displayMessage(const u_int8_t* message);
void breakAllPthreads(nodeList* head);

int main() {

    nodeList* pthreadsList = NULL;

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for (size_t i = 0; i < BUFFER_SIZE; ++i) {
        appendRing(&ringQueue, false);
    }
        
    printf("Current buffer size = %lu\n", ringQueue->size);
    bool cycleFlag = true;

    do {
        char ch = getchar();
        switch(ch) {
            case 'p' : {
                pthread_t pthreadProducer;
                pthread_create(&pthreadProducer, NULL, producer, NULL);
                pushList(&pthreadsList, pthreadProducer, 'P');
                break;
            }
            case 'c' : {
                pthread_t pthreadConsumer;
                pthread_create(&pthreadConsumer, NULL, consumer, NULL);
                pushList(&pthreadsList, pthreadConsumer, 'C');
                break;
            }
            case 'l' : {
                displayList(pthreadsList);
                break;
            }
            case 'r' : {
                displayRing(ringQueue);
                break;
            }
            case 'k' : {
                size_t num;
                scanf("%lu", &num);
                pthread_t id = eraseList(&pthreadsList, num);
                if(id = -1) {
                    break; 
                }
                pthread_kill(id, SIGUSR1);
                break;
            }
            case '+' : {
                pthread_mutex_lock(&mutex);
                appendRing(&ringQueue, true);
                if (ringQueue != NULL) {
                    printf("Inserted, current buffer size = %lu\n", ringQueue->size);
                }
                CNT_FREE_PLACES++;
                pthread_mutex_unlock(&mutex);
                break;
            }
            case '-' : {
                pthread_mutex_lock(&mutex);
                sleep(2);
                bool flag_execute = eraseRing(&ringQueue);
                if (flag_execute == false) {
                    producerPasses++;
                    CNT_FREE_PLACES--;
                }
                if (flag_execute == true) {
                    CNT_FILL_PLACES--;
                }
                if (ringQueue != NULL) {
                    printf("Deleted, current buffer size = %lu\n", ringQueue->size);
                } else {
                    printf("Ring is empty\n");
                }
                pthread_mutex_unlock(&mutex);
                break;
            }
            case 'q' : {
                cycleFlag = false;
                breakProcessFlag = true;
                pthread_cond_broadcast(&cond);
                break;
            }
            default : {
                printf("Incorrect input.\n");
                fflush(stdin); 
                break;
            }
        }
        getchar();
    }while(cycleFlag);

    breakAllPthreads(pthreadsList);

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);

    return 0;
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
    for (size_t i = 0; i < messageSize; ++i) {
        printf("%02X", message[i]);
    } 
    printf("\n");
}

void handlerStopProc() {
    continueFlag = false;
}

void consumer() {
    signal(SIGUSR1, handlerStopProc);
    do {
        pthread_mutex_lock(&mutex);
        while (CNT_FILL_PLACES == 0) {
            pthread_cond_wait(&cond, &mutex);
            if (breakProcessFlag == true) {
                break;
            }
        }
        sleep(2);
        u_int8_t* message = extractMessage(ringQueue);
        CNT_FILL_PLACES--;
        CNT_FREE_PLACES++;
        displayMessage(message);
        free(message);
        printf("Consumed from pthread with id = %lu\n", pthread_self());
        printf("Total messages retrieved = %lu\n", ringQueue->consumed);
        pthread_mutex_unlock(&mutex);
        usleep(50000);
    }while(continueFlag);
    pthread_join(pthread_self(), NULL);
}

void producer() {
    signal(SIGUSR1, handlerStopProc);
    do {
        pthread_mutex_lock(&mutex);
        if (CNT_FREE_PLACES == 0) {
            pthread_mutex_unlock(&mutex);
            sleep(5);
            continue;
        }
        sleep(2);
        u_int8_t* new_message = generateMessage();
        addMessage(ringQueue, new_message);
        CNT_FILL_PLACES++;
        CNT_FREE_PLACES--;
        if (CNT_FREE_PLACES == 0) {
            pthread_cond_broadcast(&cond);
        }
        free(new_message);
        printf("Produced from pthread with id = %lu\n", pthread_self());
        printf("Total ojbects created = %lu\n", ringQueue->produced);
        pthread_mutex_unlock(&mutex);
        usleep(50000);
    }while(continueFlag);
    pthread_join(pthread_self(), NULL);
}

void breakAllPthreads(nodeList* head) {
    if (head == NULL) {
        fprintf(stderr, "No pthreads to break\n");
        return;
    }
    while(head != NULL) {
        pthread_cancel(head->pthreadId);
        head = head->next;
    }
}