//
// Created by Adraa on 3. 1. 2022.
//

#include "socketManager.h"

int readSocketServer(char *buffer, int n, int newsockfd){
    bzero(buffer,256);
    n = read(newsockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 4;
    }
    return 0;
}

int writeSocketServer(int n, int newsockfd, char* msg){
    n = write(newsockfd, msg, strlen(msg)+1);
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }
    return 0;
}

int readSocketClient(int n, int sockfd, char* buffer) {
    bzero(buffer,256);
    n = read(sockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }
}

int writeSocketClient(int n, int sockfd, char* buffer) {
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }
    return 0;
}

void sendFile(FILE *fp1, int sockfd, int n) {
    char data[1000] = {0};

    while(fgets(data, 1000, fp1) != NULL) {
        writeSocketClient(n, sockfd, data);
    }
    writeSocketClient(n, sockfd, "end");
    bzero(data, 1000);
}