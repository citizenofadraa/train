//
// Created by Adraa on 3. 1. 2022.
//

#ifndef TRAIN_FILEMANAGER_H
#define TRAIN_FILEMANAGER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

extern FILE* fp;

void writeToFile(char* tokenLogin, char* tokenPassword);
void writeToFileContact(char* contacts[], int size);
void createContacts();
void createUsers();
void registerContacts();
int getLinesCount(char* file);
void readFile(char* array[], char* path);
void rewriteFile(char* users[], int size);

#endif //TRAIN_FILEMANAGER_H
