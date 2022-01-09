//
// Created by Adraa on 3. 1. 2022.
//

#ifndef TRAIN_FILEMANAGER_H
#define TRAIN_FILEMANAGER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void writeToFile(char* tokenLogin, char* tokenPassword);
void writeToFileContact(char* contacts[], int size);
void createContacts();
void createUsers();
void createMessages();
void registerContacts();
int getLinesCount(char* file);
void readFileUsers(char* array[]);
void readFileContacts(char* array[]);
void readFileMessages(char* array[]);
void rewriteFile(char* array[], int size, char* path);
void writeMessage(char* message);

#endif //TRAIN_FILEMANAGER_H
