//
// Created by Adraa on 3. 1. 2022.
//

#ifndef TRAIN_SIGNINGMANAGER_H
#define TRAIN_SIGNINGMANAGER_H

#include "socketManager.h"
#include "fileManager.h"
#include <stdbool.h>
#include <stdlib.h>

int signInClient(char* buffer, int n, int sockfd);
int signInServer(char* buffer, int n, int newsockfd, int size, char* users[]);

#endif //TRAIN_SIGNINGMANAGER_H
