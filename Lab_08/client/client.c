#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define MAX_LEN_BUFFER 1024
#define QUIT_QUERY "QUIT"

bool is_from_file = false;
char SERVER_RESPONSE[MAX_LEN_BUFFER];
char QUERY[MAX_LEN_BUFFER];


bool isEndSession() {
    if (strncasecmp(QUERY, QUIT_QUERY, strlen(QUIT_QUERY)) == 0) {
        return true;
    }
    return false;
}

void sendQuery(int clientSocketDescriptor, bool* endSessionFlag) {
    write(clientSocketDescriptor, QUERY, MAX_LEN_BUFFER);
    *endSessionFlag = isEndSession();
    ssize_t bytesRead = recv(clientSocketDescriptor, QUERY, MAX_LEN_BUFFER, 0);
    QUERY[bytesRead] = '\0';
    if (bytesRead > 0) {
        fputs(QUERY, stdout);
    } else {
        fprintf(stderr, "The server dropped the connection\n");
        close(clientSocketDescriptor);
        exit(EXIT_FAILURE);
    }
}

void sendQueryFromFile(int clientSocketDescriptor, bool* endSessionFlag) {
    FILE* file = fopen(QUERY + 1, "r");
    if (file == NULL) {
        printf("File %s is not found\n", QUERY + 1);
        return;
    }
    while(!feof(file)) {
        memset(QUERY, '\0', strlen(QUERY));
        fgets(QUERY, MAX_LEN_BUFFER, file);
        printf("> %s", QUERY);
        sendQuery(clientSocketDescriptor, endSessionFlag);
        if (*endSessionFlag == true) {
            return;
        }
    }
}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    struct addrinfo hints = {0};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    struct addrinfo* result = NULL;

    int status = getaddrinfo(NULL, argv[1], &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    int clientSocketDescriptor = socket(hints.ai_family, hints.ai_socktype, hints.ai_addrlen);

    if (connect(clientSocketDescriptor, result->ai_addr, result->ai_addrlen) == -1) {
        printf("Server is closed\n");
        exit(EXIT_FAILURE);
    }

    read(clientSocketDescriptor, SERVER_RESPONSE, MAX_LEN_BUFFER);
    printf("%s", SERVER_RESPONSE);

    bool endSessionFlag = false;
    while(endSessionFlag == false) {
        printf("> ");
        fgets(QUERY,MAX_LEN_BUFFER, stdin);
        if (QUERY[0] == '@') {
            QUERY[strlen(QUERY) - 1] = '\0';
            sendQueryFromFile(clientSocketDescriptor, &endSessionFlag);
        } else {
            sendQuery(clientSocketDescriptor, &endSessionFlag);
        }
    }

    close(clientSocketDescriptor);
    return 0;
}