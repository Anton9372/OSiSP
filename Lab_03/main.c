#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include "list.h"

#define COUNT_OF_REPEATS 101
#define SEC_TIMER 16000
#define SEC_FOR_PREFF_G 5

typedef struct pair {
    int first;
    int second;
}pair;

typedef struct stats
{
    int cnt00;
    int cnt01;
    int cnt10;
    int cnt11;
}stats;

size_t cnt = 0;
stats valStats;
pair valPair;
bool continueCollect = true;
bool collect = true;
pid_t pidParent;
node* head;

void allowStatsOutput();
void disableStatsOutput();
void showStats();
void takeStats();
void deleteLastProc();
void makeStats();
void createProc();
void deleteAllChildProcs();
void commandToShowStatsForNProc(size_t, bool, bool);
void commandToShowStatsForAllProcs(bool);
void allowAllAfterP();

int main() {
    int num = 0;
    signal(SIGINT, showStats);
    signal(SIGUSR1, allowStatsOutput);
    signal(SIGUSR2, disableStatsOutput);
    bool flagСontinue = true;
    pidParent = getpid();
    pushList(&head, pidParent);
    do {
        int ch = getchar();
        switch(ch) {
            case '+' : { 
                createProc(); 
                break; 
            }
            case '-' : { 
                deleteLastProc(); 
                break; 
            }
            case 'l' : { 
                displayList(head); 
                break;
            }
            case 'k' : { 
                deleteAllChildProcs(); 
                break; 
            }
            case 's' : {
                printf("Enter 0 for all processes, or another number for <num> process: ");
                scanf("%d", &num);
                if (num == 0) {
                    commandToShowStatsForAllProcs(false);
                } else {
                    commandToShowStatsForNProc(num, false, false);
                }
                break;
            }
            case 'g' : {
                printf("Enter 0 for all processes, or another number for <num> process: ");
                scanf("%d", &num);
                if (num == 0) {
                    commandToShowStatsForAllProcs(true);
                } else {
                    commandToShowStatsForNProc(num, true, false);
                }
                break;
            }
            case 'p' : {
                commandToShowStatsForAllProcs(false);
                printf("Enter the number of the process that will display statistics: ");
                scanf("%d", &num);
                commandToShowStatsForNProc(num, true, true);
                signal(SIGALRM, allowAllAfterP);
                alarm(SEC_FOR_PREFF_G);
                break;
            }
            case 'q' : { 
                deleteAllChildProcs();
                flagСontinue = false; 
                break; 
            }
            default : { 
                flagСontinue = false; 
                break; 
            }
        }
        getchar();
    }while(flagСontinue);
    clearList(&head);
    return 0;
}

void commandToShowStatsForNProc(size_t n, bool allowFlag, bool queryFlag) {
    if (cnt >= n) {
        size_t i = 1;
        node* current = head->next;
        while(i++ != n) {
            current = current->next;
        }
        if (queryFlag) {
            kill(current->pid, SIGINT);
            return;
        }
        if (allowFlag) {
            kill(current->pid, SIGUSR1);
        } else {
            kill(current->pid, SIGUSR2);
        }
    } else {
        printf("There is no child process with this number.\n");
    }
}

void allowStatsOutput() {
    collect = true;
}

void disableStatsOutput() {
    collect = false;
}

void showStats() {
    printf("Statistic of child process with PID = %d, PPID = %d All values: ", getpid(), getppid());
    printf("{00} : %d, {01} : %d, {10} : %d, {11} : %d, ", valStats.cnt00, valStats.cnt01, valStats.cnt10, valStats.cnt11);
    printf("\n");
    valStats.cnt00 = valStats.cnt01 = valStats.cnt10 = valStats.cnt11 = 0; 
}

void takeStats() {
    if (valPair.first == valPair.second) {
        if (valPair.first == 1) {
            valStats.cnt11++;
        } else {
            valStats.cnt00++;
        }
    } else {
        if (valPair.first == 0) {
            valStats.cnt01++;
        } else {
            valStats.cnt10++;
        }
    }
    continueCollect = false;
}

void deleteLastProc() {
    if (head->next == NULL) {
        printf("No child processes.\n");
        return;
    }
    pid_t pid = popList(&head);
    kill(pid, SIGKILL);
    printf("Child process with PID = %d successfully deleted.\n", pid);
    printf("Remaining number of child processes: %lu\n", --cnt);
}

void makeStats() {
    do {
        for (size_t i = 0; i < COUNT_OF_REPEATS; ++i) {
            ualarm(SEC_TIMER, 0);
            size_t j = 0;
            do {
                valPair.first = j % 2;
                valPair.second = j % 2;
                j++;
            }while(continueCollect);
            continueCollect = true;
        }
        if (collect)
          showStats();
    }while(1);
}

void createProc() {
    pid_t pid = fork();
    if (pid == 0) {
        signal(SIGALRM, takeStats);
        makeStats();
    }
    else if(pid == -1) {
        fprintf(stderr, "Failed to create child process");
        exit(EXIT_FAILURE);
    }
    else {
        pushList(&head, pid);
        cnt++;
        printf("Child process with PID = %d spawned successfully.\n", pid);
    }
}

void commandToShowStatsForAllProcs(bool allowFlag) {
    if (head->next != NULL) {
        node* current = head->next;
        while(current != NULL) {
            if (allowFlag == true) {
                kill(current->pid, SIGUSR1);
            } else {
                kill(current->pid, SIGUSR2);
            }
            current = current->next;
        }
    }
}

void deleteAllChildProcs() {
    while(head->next != NULL) {
        pid_t pid = popList(&head);
        kill(pid, SIGKILL);
        cnt--;
    }    
    printf("All child processes are deleted.\n");
}

void allowAllAfterP() {
    commandToShowStatsForAllProcs(true);
}