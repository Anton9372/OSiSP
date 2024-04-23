#include "server_lib.h"

char defaultDirectoryPath[MAX_LEN_BUFFER];
const char *infoFileName;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s [port], [directory], [fileName with serverInfo]\n", argv[0]);
        exit(-1);
    }

    infoFileName = argv[3];

    DIR* directory = opendir(argv[2]);
    if (directory == NULL) {
        printf(INVALID_DIRECTORY);
        exit(-2);
    }
    getcwd(defaultDirectoryPath, MAX_LEN_BUFFER);

    struct addrinfo hints;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

    struct addrinfo *result = NULL;

    int status = getaddrinfo(NULL, argv[1], &hints, &result);
    if (status!= 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(EXIT_FAILURE);
    }

    int serverSocketDescriptor = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(serverSocketDescriptor, result->ai_addr, result->ai_addrlen);

    printf("My server on port %s is ready\n", argv[1]);

    listen(serverSocketDescriptor, SOMAXCONN);

    while (1) {
        int clientSocketDescriptor = accept(serverSocketDescriptor, result->ai_addr, &result->ai_addrlen);
        if (clientSocketDescriptor == -1) {
            fprintf(stderr, "Accept server error\n");
            exit(-228);
        }

        pthread_t thread;
        if(pthread_create(&thread, NULL, handleClient, (void*)&clientSocketDescriptor) != 0) {
            fprintf(stderr, "Accept client error, client descriptor: %d\n", clientSocketDescriptor);
            exit(-228);
        }

        pthread_detach(thread);
    }

    close(serverSocketDescriptor);

    return 0;
}