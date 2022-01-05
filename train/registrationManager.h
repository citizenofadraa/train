//
// Created by Adraa on 3. 1. 2022.
//

#ifndef TRAIN_REGISTRATIONMANAGER_H
#define TRAIN_REGISTRATIONMANAGER_H

#include "socketManager.h"
#include "fileManager.h"
#include <stdbool.h>
#include <stdlib.h>

int registerUserServer(char* buffer, int n, int newsockfd, int size, char* users[]);
int registerUserClient(char* buffer, int n, int sockfd);

#endif //TRAIN_REGISTRATIONMANAGER_H
