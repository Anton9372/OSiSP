#include "server_lib.h"

char* readDirectoryContent(DIR* directory, char* currentDirectoryPath) {

    char* directoryContent = (char*)malloc(MAX_LEN_BUFFER * sizeof(char));

    struct dirent *dirEntry = NULL;
    struct stat dirEntryInfo;

    memset(directoryContent, '\0', strlen(directoryContent));

    strncat(directoryContent, currentDirectoryPath, strlen(currentDirectoryPath));
    strncat(directoryContent, "\n", strlen("\n"));

    while ((dirEntry = readdir(directory))) {
        char fullPath[MAX_LEN_BUFFER];
        
        if (strcmp(dirEntry->d_name, ".") == 0 || strcmp(dirEntry->d_name, "..") == 0) {
            continue;
        }
        
        sprintf(fullPath, "%s/%s", currentDirectoryPath, dirEntry->d_name);
        
        if (lstat(fullPath, &dirEntryInfo) == -1) {
            continue;
        }
        
        if ((dirEntryInfo.st_mode & __S_IFMT) == __S_IFDIR) {
            strncat(directoryContent, dirEntry->d_name, strlen(dirEntry->d_name));
            strncat(directoryContent, "/", strlen("/"));

        } else if ((dirEntryInfo.st_mode & __S_IFMT) == __S_IFREG) {
            strncat(directoryContent, dirEntry->d_name, strlen(dirEntry->d_name));

        } else if ((dirEntryInfo.st_mode & __S_IFMT) == __S_IFLNK) {
            char pathToTarget[MAX_LEN_BUFFER];
            memset(pathToTarget, '\0', strlen(pathToTarget));
            if (readlink(fullPath, pathToTarget, sizeof(pathToTarget) - 1) != -1) {
                struct stat targetInfo;
                if(lstat(pathToTarget, &targetInfo) == -1) {
                    fprintf(stderr, "Lstat error. Path : %s\n", pathToTarget);
                    continue;
                }
                if ((targetInfo.st_mode & __S_IFMT) == __S_IFREG) {
                    strncat(directoryContent, dirEntry->d_name, strlen(dirEntry->d_name));
                    strncat(directoryContent, LINK_TO_FILE, strlen(LINK_TO_FILE));
                    strncat(directoryContent, pathToTarget, strlen(pathToTarget));
                } else if ((targetInfo.st_mode & __S_IFMT) == __S_IFLNK) {
                    strncat(directoryContent, dirEntry->d_name, strlen(dirEntry->d_name));
                    strncat(directoryContent, LINK_TO_LINK, strlen(LINK_TO_LINK));
                    strncat(directoryContent, pathToTarget, strlen(pathToTarget));
                }
            }
        }

        strncat(directoryContent, "\n", strlen("\n"));
    }
    closedir(directory);
    return directoryContent;
}

size_t readFileInfo(const char* fileName, char** buffer) {
    size_t size = 0;
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("The file was not found: %s\n", fileName);
        return -1;
    }
    if (buffer == NULL) {
        fclose(file);
        return -1;
    }
    fseek(file, 0, SEEK_END);
    size = ftell(file);
    fseek(file, 0, SEEK_SET);
    *buffer = (char*)realloc(*buffer, size * sizeof(char));
    fread(*buffer, sizeof(char), size, file);
    fclose(file);
    return size;
}

void consoleQueryLog(int clientSocketDescriptor, const char *serverCommand, const char *clientCommand, int loggerLevel) {
    const time_t loggingDateTime = time(NULL);
    char loggingDateTimeString[LEN_DATA];
    strftime(loggingDateTimeString, LEN_DATA, "%d.%m.%Y %H:%M:%S ", localtime(&loggingDateTime));

    switch (loggerLevel) {
        case FATAL_: {
            printf("\033[1;31m%sFATAL\033[0m %s from [%d] with arguments = %s",
                   loggingDateTimeString, serverCommand, clientSocketDescriptor, clientCommand);
            break;
        }
        case ERROR_: {
            printf("\033[1;31m%sERROR\033[0m %s from [%d] with arguments = %s",
                   loggingDateTimeString, serverCommand, clientSocketDescriptor, clientCommand);
            break;
        }
        case WARN_: {
            printf("\033[1;33m%sWARN\033[0m %s from [%d] with arguments = %s",
                   loggingDateTimeString, serverCommand, clientSocketDescriptor, clientCommand);
            break;
        }
        case INFO_: {
            printf("\033[1;32m%sINFO\033[0m %s from [%d] with arguments = %s",
                   loggingDateTimeString, serverCommand, clientSocketDescriptor, clientCommand);
            break;
        }
        case DEBUG_: {
            printf("\033[1;34m%sDEBUG\033[0m %s from [%d] with arguments = %s",
                   loggingDateTimeString, serverCommand, clientSocketDescriptor, clientCommand);
            break;
        }
        case TRACE_: {
            printf("\033[1;36m%sTRACE\033[0m %s from [%d] with arguments = %s",
                   loggingDateTimeString, serverCommand, clientSocketDescriptor, clientCommand);
            break;
        }
        default: {
            break;
        }
    }
}

void* handleClient(void *clientSocketDescriptor_) {

    char QUERY[MAX_LEN_BUFFER];
    char currentDirectoryPath[MAX_LEN_BUFFER];
    char directoryContent[MAX_LEN_BUFFER];
    strncpy(currentDirectoryPath, defaultDirectoryPath, strlen(defaultDirectoryPath));

    int clientSocketDescriptor = *(int*)clientSocketDescriptor_;

    DIR* directory = opendir(currentDirectoryPath);
    if (directory == NULL) {
        printf(INVALID_DIRECTORY);
        exit(-2);
    }

    char *serverInfo = NULL;
    size_t fileSize = readFileInfo(infoFileName, &serverInfo);
    write(clientSocketDescriptor, serverInfo, fileSize);

    while (true) {
        memset(QUERY, '\0', strlen(QUERY));
        ssize_t bytesRead = recv(clientSocketDescriptor, QUERY, MAX_LEN_BUFFER, 0);
        size_t queryStrlen = strlen(QUERY);
        if (bytesRead == 0) {
            printf("Client [%d] closed the connection\n", clientSocketDescriptor);
            break;
        } else if (bytesRead < 0) {
            fprintf(stderr, "Data from client reding error\n");
            continue;
        }

        if (strncasecmp(QUERY, ECHO_QUERY, strlen(ECHO_QUERY)) == 0) {
            consoleQueryLog(clientSocketDescriptor, ECHO_QUERY, QUERY, INFO_);
            write(clientSocketDescriptor, QUERY + strlen(ECHO_QUERY) + 1, queryStrlen - strlen(ECHO_QUERY) - 1);
        } 
        
        else if (strncasecmp(QUERY, INFO_QUERY, strlen(INFO_QUERY)) == 0) {
            consoleQueryLog(clientSocketDescriptor, INFO_QUERY, QUERY, INFO_);
            write(clientSocketDescriptor, serverInfo, fileSize);
        } 
        
        else if (strncasecmp(QUERY, CD_QUERY, strlen(CD_QUERY)) == 0) {
            consoleQueryLog(clientSocketDescriptor, CD_QUERY, QUERY, INFO_);

            QUERY[queryStrlen - 1] = '\0';
            strcat(currentDirectoryPath, "/");
            strcat(currentDirectoryPath, QUERY + 3);

            if ((opendir(currentDirectoryPath)) == NULL) {
                write(clientSocketDescriptor, INVALID_DIRECTORY, strlen(INVALID_DIRECTORY));
            } else {
                directory = opendir(currentDirectoryPath);
                QUERY[queryStrlen - 1] = '\n';
                write(clientSocketDescriptor, QUERY + 3, queryStrlen - 3);
            }
        }

        else if (strncasecmp(QUERY, LIST_QUERY, strlen(LIST_QUERY)) == 0) {
            consoleQueryLog(clientSocketDescriptor, LIST_QUERY, QUERY, INFO_);
            char* result = readDirectoryContent(directory, currentDirectoryPath);
            write(clientSocketDescriptor, result, strlen(result));
        } 

        else if (strncasecmp(QUERY, QUIT_QUERY, strlen(QUIT_QUERY)) == 0) {
            consoleQueryLog(clientSocketDescriptor,QUIT_QUERY, QUERY, INFO_);
            write(clientSocketDescriptor, EXIT_MESSAGE, strlen(EXIT_MESSAGE));
        } 
        
        else {
            consoleQueryLog(clientSocketDescriptor, UNKNOWN_QUERY, QUERY, ERROR_);
            write(clientSocketDescriptor, INVALID_QUERY, strlen(INVALID_QUERY));
        }
        
    }

    free(serverInfo);
    close(clientSocketDescriptor);
    pthread_exit(NULL);
}