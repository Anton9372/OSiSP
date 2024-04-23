#include "lib.h"

int compare(const void *a, const void *b) {
    return (double)((indexRecord*) a)->timeMark - (double)((indexRecord *) b)->timeMark;
}

void* sorting(void* threadArgs_) {
    threadArgs* args = (threadArgs*)threadArgs_;
    pthread_barrier_wait(&barrier);

    while(currentMemsizeBlock < args->bufferBegin + memsize) {
        pthread_mutex_lock(&mutex);
        if(currentMemsizeBlock < args->bufferBegin + memsize) {
            indexRecord *temp = currentMemsizeBlock;
            currentMemsizeBlock += args->blockSize;
            pthread_mutex_unlock(&mutex);
            
            qsort(temp, args->blockSize, sizeof(indexRecord), compare);
        } else {
            pthread_mutex_unlock(&mutex);
            pthread_barrier_wait(&barrier);
            break;
        }
    }

    merge(args);
}

void merge(void* threadArgs_) {
    threadArgs* args = (threadArgs*)threadArgs_;
    int mergeStep = 2;

    while (mergeStep <= numOfBlocks) {   
        pthread_barrier_wait(&barrier);
        currentMemsizeBlock = args->bufferBegin;

        while (currentMemsizeBlock < args->bufferBegin + memsize) {
            pthread_mutex_lock(&mutex);

            if (currentMemsizeBlock < args->bufferBegin + memsize) {
                indexRecord *temp = currentMemsizeBlock;
                currentMemsizeBlock += mergeStep * args->blockSize;
                pthread_mutex_unlock(&mutex);

                int sizeOfOneBlockToMerge = (mergeStep / 2) * args->blockSize;
                twoBlocksMerge(temp, sizeOfOneBlockToMerge);
            } else {
                pthread_mutex_unlock(&mutex);
                break;
            }
        }
        mergeStep *= 2;
    }
    pthread_mutex_unlock(&mutex);
    pthread_barrier_wait(&barrier);
}

void twoBlocksMerge(indexRecord* firstBlockBegin, int sizeOfOneBlockToMerge) {
    indexRecord *left = (indexRecord *)malloc(sizeOfOneBlockToMerge * sizeof(indexRecord));
    indexRecord *right = (indexRecord *)malloc(sizeOfOneBlockToMerge * sizeof(indexRecord));
    memcpy(left, firstBlockBegin, sizeOfOneBlockToMerge * sizeof(indexRecord));
    memcpy(right, firstBlockBegin + sizeOfOneBlockToMerge, sizeOfOneBlockToMerge * sizeof(indexRecord));
    int leftIdx = 0;
    int rightIdx = 0;
    int idx = 0;

    while (leftIdx < sizeOfOneBlockToMerge && rightIdx < sizeOfOneBlockToMerge) {
        if (left[leftIdx].timeMark > right[rightIdx].timeMark) {
            firstBlockBegin[idx].timeMark = right[rightIdx].timeMark;
            firstBlockBegin[idx].recno = right[rightIdx].recno;
            rightIdx++;
            idx++;
        } else {
            firstBlockBegin[idx].timeMark = left[leftIdx].timeMark;
            firstBlockBegin[idx].recno = left[leftIdx].recno;
            leftIdx++;
            idx++;
        }
    }

    while (leftIdx < sizeOfOneBlockToMerge) {
        firstBlockBegin[idx].timeMark = left[leftIdx].timeMark;
        firstBlockBegin[idx].recno = left[leftIdx].recno;
        leftIdx++;
        idx++;
    }

    while (rightIdx < sizeOfOneBlockToMerge) {
        firstBlockBegin[idx].timeMark = right[rightIdx].timeMark;
        firstBlockBegin[idx].recno = right[rightIdx].recno;
        rightIdx++;
        idx++;
    }
    
    free(left);
    free(right);
}