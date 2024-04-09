#include "ring.h"

#include <inttypes.h>


nodeRing* constructorNode() {
    nodeRing* temp = (nodeRing*)malloc(1 * sizeof(nodeRing));
    temp->next = temp;
    temp->prev = temp;
    temp->flagIsBusy = false;
    return temp;
}

ringBuffer* constructorBuffer() {
    ringBuffer* temp = (ringBuffer*)malloc(1 * sizeof(ringBuffer));
    temp->tail = NULL;
    temp->head = NULL;
    temp->consumed = 0;
    temp->produced = 0;
    temp->size = 0;
    return temp;
}

void appendRing(ringBuffer** ring, bool bufferIsWorkingNow) {
    if (ring == NULL)
        exit(-100);
    if (*ring == NULL) {
        *ring = constructorBuffer();
    }
    nodeRing* temp = constructorNode();
    (*ring)->size ++;
    if ((*ring)->head == NULL) {
        (*ring)->head = temp;
        (*ring)->tail = temp;
        return;
    }
    nodeRing* curr = (*ring)->head;        
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
    //change to curr
    if(bufferIsWorkingNow == true) {
        if(temp->next == (*ring)->tail) {
            if((*ring)->tail == (*ring)->head && 
                (*ring)->head->flagIsBusy == false) {
                    (*ring)->tail = (*ring)->head = temp;
            } else {
                (*ring)->tail = temp;
            }
        }
    }
}

int eraseRing(ringBuffer** ring) {
    if(ring == NULL) {
        exit(-100);
    }
    if(*ring == NULL || (*ring)->head == NULL) {
        fprintf(stderr, "Ring is empty\n");
        return -1;
    }
    (*ring)->size--;
    bool result;
    if((*ring)->head == (*ring)->tail) {
        if((*ring)->head == (*ring)->head->next) {
            result = (*ring)->head->flagIsBusy;
            free((*ring)->head);
            free(*ring);
            *ring = NULL;
        } else {
            nodeRing* temp = (*ring)->head;
            (*ring)->head->next->prev = (*ring)->head->prev;
            (*ring)->head->prev->next = (*ring)->head->next;
            (*ring)->head = (*ring)->tail = (*ring)->head->next;
            result = temp->flagIsBusy;
            free(temp);
        }
        return result;
    }
    if((*ring)->head->next == (*ring)->head->prev) {
        (*ring)->tail = (*ring)->tail->prev;
        result = (*ring)->tail->next->flagIsBusy;
        free((*ring)->tail->next);
        (*ring)->tail->next = (*ring)->tail->prev = (*ring)->tail;
        return result;
    } else {
        nodeRing* temp = (*ring)->tail;
        (*ring)->tail->next->prev = (*ring)->tail->prev;
        (*ring)->tail->prev->next = (*ring)->tail->next;
        (*ring)->tail = (*ring)->tail->next;
        result = temp->flagIsBusy;
        free(temp);
        return result;
    }
}

void addMessage(ringBuffer* ring, const u_int8_t* message) {
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
        
    curr->flagIsBusy = true;
    ring->tail = curr->next;
    ring->produced++;
}

u_int8_t* extractMessage(ringBuffer* ring) {
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

    curr->flagIsBusy = false;    
    ring->head = curr->next;
    ring->consumed++;
    return message;
}

void displayRing(ringBuffer* ring) {
     if (ring == NULL) {
        return;
    }
    nodeRing* curr = ring->head;
    do{
        printf("(curr=%d)", curr->flagIsBusy);
        printf("-->next=%d", curr->next->flagIsBusy);
        curr = curr->next;
    }while(curr != ring->head);
    printf("\n");
}

void clearRing(ringBuffer* ring) {  
    if (ring == NULL) {
        return;
    }
    if(ring->size == 0) {
        return;
    }
    if(ring->size == 1) {
        free(ring);
        return;
    }
    size_t size = ring->size;
    nodeRing* curr = ring->head;
    while(size - 1) {
        free(curr->prev);
        curr = curr->next;
        size--;
    }
    free(curr);
}