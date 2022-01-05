//
// Created by Adraa on 3. 1. 2022.
//

#ifndef TRAIN_SOCKETMANAGER_H
#define TRAIN_SOCKETMANAGER_H

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

int readSocketServer(char *buffer, int n, int newsockfd);
int writeSocketServer(int n, int newsockfd, char* msg);
int readSocketClient(int n, int sockfd, char* buffer);
int writeSocketClient(int n, int sockfd, char* msg);

#endif //TRAIN_SOCKETMANAGER_H
