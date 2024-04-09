#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_LEN_RECORD 80
#define COUNT_RECORDS 10

typedef struct record {
    char name[MAX_LEN_RECORD];
    char address[MAX_LEN_RECORD];
    u_int8_t semester;
}record;

int descriptor = 0;
bool FLAG_EDIT = false;

#define DELAY 10

bool isEqual(const record* a, const record* b);
void recordCopy(record* to, const record* from);
void readAllRecords();
bool getNumRecord(size_t recordNum, record* record_);
void modifyNumRecord(size_t recordNum, record* record_, record* savedRecord);
bool putNumRecord(record* newRecord, const record* record_, const record* savedRecord, size_t recordNum);

int main(int argc, char* argv[])
{
    bool cycleFlag = true;
    record REC;
    record SAVED_REC;
    record NEW_REC;
    size_t REC_NUM = 0;

    descriptor = open("file.bin", O_RDWR);

    if (descriptor == -1) {
        printf("ERROR: Wrong file name.\n");
        exit(0);
    }

    do {
        printf("1.Display all records.\n");
        printf("2.Modify record.\n");
        printf("3.Get num_record.\n");
        printf("4.Put last modified record.\n");
        printf("5.Exit.\n");
        char ch = getchar();
        switch(ch) {
            case '1': {
                readAllRecords();
                break;
            }
            case '2' : {
                printf("Enter the num of record: ");
                scanf("%lu", &REC_NUM);
                modifyNumRecord(REC_NUM, &REC, &SAVED_REC);
                break;
            }
            case '3' : {
                record record;
                size_t num;
                printf("Enter the num of record: ");
                scanf("%lu", &num);
                getNumRecord(num, &record);
                printf("%lu. Name: %s, Address: %s, Num of semester: %hhu\n", num, record.name, record.address, record.semester);
                break;
            }
            case '4' : {
                if (!FLAG_EDIT) {
                    printf("No entry has been changed!\n");
                } else {
                    if (putNumRecord(&NEW_REC, &REC, &SAVED_REC, REC_NUM) == false) {
                        printf("The data has been changed by someone, please repeat the editing operation again.\n");
                        modifyNumRecord(REC_NUM, &REC, &SAVED_REC);
                    }
                }
                break;
            }
            case '5' : {
                cycleFlag = false;
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

    int status_close = close(descriptor);
    if (status_close == -1) {
        printf("Unable to close file.\n");
        exit(0);
    }

    close(descriptor);

    return 0;
}

bool isEqual(const record* a, const record* b) {
    if (a == NULL || b == NULL) {
        printf("Empty records to compare\n");
        exit(-100);
    }
    if (strcmp(a->name, b->name) == 0 &&
        strcmp(a->address, b->address) == 0 &&
        a->semester == b->semester) {
            return true;
        }   
    return false;
}

void recordCopy(record* to, const record* from) {
    if (to == NULL || from == NULL) {
        printf("Empty records to copy\n");
        exit(-100);
    } 
    strncpy(to->name, from->name, MAX_LEN_RECORD);
    strncpy(to->address, from->address, MAX_LEN_RECORD);
    to->semester = from->semester;
}

void readAllRecords() {
    struct flock parameters;
    parameters.l_type = F_RDLCK;
    parameters.l_whence = SEEK_SET;
    parameters.l_start = 0;
    parameters.l_len = 0;
    if(fcntl(descriptor, F_SETLKW, &parameters) < 0) {
        perror("F_SETLKW");
    }
    record buffer;
    for (size_t i = 0; i < COUNT_RECORDS; ++i) {
        read(descriptor, &buffer, sizeof(record));
        printf("%lu. Name: %s, Address: %s, Num of semester: %hhu\n", i+1, buffer.name, buffer.address, buffer.semester);
    }
    sleep(DELAY);
    parameters.l_type = F_UNLCK;
    if(fcntl(descriptor, F_SETLKW, &parameters) < 0) {
        perror("F_SETLKW");
    }
    lseek(descriptor, 0, SEEK_SET);
}

bool getNumRecord(size_t recordNum, record* record_) {
    if (recordNum > COUNT_RECORDS || recordNum == 0) {
        return false;
    }
    struct flock parameters;
    parameters.l_type = F_RDLCK;
    parameters.l_whence = SEEK_SET;
    parameters.l_start = (recordNum - 1) * sizeof(record);
    parameters.l_len = sizeof(record);
    if(fcntl(descriptor, F_SETLKW, &parameters) < 0) {
        perror("F_SETLKW");
    }
    lseek(descriptor, (recordNum - 1) * sizeof(record), SEEK_SET);
    read(descriptor, record_, sizeof(record));
    parameters.l_type = F_UNLCK;
    if(fcntl(descriptor, F_SETLKW, &parameters) < 0) {
        perror("F_SETLKW");
    }
    lseek(descriptor, 0, SEEK_SET);
    return true;
}

void modifyNumRecord(size_t recordNum, record* record_, record* savedRecord) {
    if (!getNumRecord(recordNum, record_)) {
        printf("No record founded\n");
        return;
    }
    recordCopy(savedRecord, record_);
    bool continueFlag = true;
    getchar();
    do {
        printf("1.Edit name.\n");
        printf("2.Edit address.\n");
        printf("3.Edit num of semester.\n");
        printf("4.Display edit record.\n");
        printf("5.Main menu\n");
        char ch = getchar();
        char buffer[MAX_LEN_RECORD];
        switch(ch) {
            case '1': {
                FLAG_EDIT = true;
                printf("Enter the name: ");
                scanf("%s", buffer);
                strncpy(record_->name, buffer, MAX_LEN_RECORD);
                rewind(stdin);
                break;
            }
            case '2': {
                FLAG_EDIT = true;
                printf("Enter the address: ");
                scanf("%s", buffer);
                strncpy(record_->address, buffer, MAX_LEN_RECORD);
                break;
            }
            case '3': {
                FLAG_EDIT = true;
                u_int8_t sem;
                printf("Enter the num of semester: ");
                scanf("%hhu", &sem);
                record_->semester = sem;
                break;
            }
            case '4' : {
                printf("%lu. Name: %s, Address: %s, Num of semester: %hhu\n", 
                    recordNum, record_->name, record_->address, record_->semester);
                break;
            }
            case '5' : {
                continueFlag = false;
                break;
            }
            default: {
                printf("Incorrect input.\n");
                fflush(stdin); 
                break;
            }
        }
        getchar();
    }while(continueFlag);
    if (isEqual(record_, savedRecord)) {
        FLAG_EDIT = false;
    }
}

bool putNumRecord(record* newRecord, const record* record_, const record* savedRecord, size_t recordNum) {
    struct flock parameters;
    parameters.l_type = F_WRLCK;
    parameters.l_whence = SEEK_SET;
    parameters.l_start = (recordNum - 1) * sizeof(record);
    parameters.l_len = sizeof(record);
    if(fcntl(descriptor, F_SETLKW, &parameters) < 0) {
        perror("F_SETLKW");
    }
    parameters.l_type = F_UNLCK;
    getNumRecord(recordNum, newRecord);
    if (!isEqual(savedRecord, newRecord)) {
        fcntl(descriptor, F_SETLK, &parameters);
        return false;
    }
    lseek(descriptor, (recordNum - 1) * sizeof(record), SEEK_SET);
    write(descriptor, record_, sizeof(record));
    if(fcntl(descriptor, F_SETLKW, &parameters) < 0) {
        perror("F_SETLKW");
    }
    lseek(descriptor, 0, SEEK_SET);
    FLAG_EDIT = false;
    return true;
}