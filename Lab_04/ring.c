#include "ring.h"

nodeRing* constructorNode() {
    int32_t shmid = shmget(0, sizeof(nodeRing), 0777);
    nodeRing* temp = shmat(shmid, NULL, 0);
    temp->shmidCurr = shmid;
    temp->next = temp;
    temp->prev = temp;
    return temp;
}

ringSharedBuffer* constructorBuffer() {
    int32_t shmid = shmget(0, sizeof(ringSharedBuffer), 0777);
    ringSharedBuffer* temp = shmat(shmid, NULL, 0);
    temp->tail = NULL;
    temp->head = NULL;
    temp->consumed = 0;
    temp->produced = 0;
    temp->shmid = shmid;
    return temp;
}

void append(ringSharedBuffer** queue) {
    if (queue == NULL)
        exit(-100);
    if (*queue == NULL) {
        *queue = constructorBuffer();
    }
    nodeRing* temp = constructorNode();
    if ((*queue)->head == NULL) {
        (*queue)->head = temp;
        (*queue)->tail = temp;
        return;
    }
    nodeRing* curr = (*queue)->head;
    if(curr == curr->next) {
        temp->next = temp->prev = curr;
        curr->next = curr->prev = temp;
    } else {
        nodeRing* prev_ = curr->prev;
        temp->next = curr;
        temp->prev = prev_;
        prev_->next = temp;
        curr->prev = temp;
    }
}

void addMessage(ringSharedBuffer* ring, const u_int8_t* message) {
    if (ring == NULL) {
        printf("The ring is empty.\n");
        return;
    }
    if (ring->head == NULL) {
        printf("There are 0 places in the ring.\n");
        return;
    }
    nodeRing* curr = ring->tail;

    for (size_t i = 0; i < LEN_MESSAGE; ++i) {
        curr->message[i] = message[i];
    }

    ring->tail = curr->next;
    ring->produced++;
}

u_int8_t* extractMessage(ringSharedBuffer* ring) {
    if (ring == NULL) {
        printf("The ring is empty.\n");
        return NULL;
    }
    if (ring->head == NULL) {
        printf("There are 0 places in the ring.\n");
        return NULL;
    }
    nodeRing* curr = ring->head;

    u_int8_t* message = (u_int8_t*)calloc(LEN_MESSAGE, sizeof(u_int8_t));

    for (size_t i = 0; i < LEN_MESSAGE; ++i) {
        message[i] =  curr->message[i];
    }
        
    ring->head = curr->next;
    ring->consumed++;
    return message;
}

void clearSharedMemory(ringSharedBuffer* ring) {
    if (ring == NULL) {
        return;
    }
    int32_t curr;
    nodeRing* buffer = ring->head;
    while (buffer->next != ring->tail) {
        curr = buffer->shmidCurr;
        int32_t shmid_next = buffer->next->shmidCurr;
        shmdt(buffer);
        shmctl(curr, IPC_RMID, NULL);
        buffer = shmat(shmid_next, NULL, 0);
    }
    curr = buffer->shmidCurr;
    shmdt(buffer);
    shmctl(curr, IPC_RMID, NULL);
    curr = ring->shmid;
    shmdt(ring);
    shmctl(curr, IPC_RMID, NULL);
}

void displayRing(ringSharedBuffer* ring) {
    if (ring == NULL) {
        return;
    }
    printf("head : %d\n", ring->head->shmidCurr);
    printf("tail : %d\n", ring->tail->shmidCurr);
    nodeRing* curr = ring->head;
    do{
        printf("(curr=%d)", curr->shmidCurr);
        printf("-->next=%d", curr->next->shmidCurr);
        curr = curr->next;
    }while(curr != ring->head);
    printf("\n");
}