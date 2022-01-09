//
// Created by Adraa on 3. 1. 2022.
//

#include "fileManager.h"

void writeToFile(char* tokenLogin, char* tokenPassword) {
    FILE *fp;
    fp = fopen("test.txt", "a");
    fprintf(fp, "%s %s\n", tokenLogin, tokenPassword);
    fclose(fp);
}

void writeToFileContact(char* contacts[], int size) {
    FILE *fp;
    fp = fopen("contacts.txt", "w+");
    for (int i = 0; i < size; ++i) {
        if (contacts[i] == NULL) {
            fprintf(fp, "\n");
        } else {
            fprintf(fp, "%s", contacts[i]);
        }
    }
    fclose(fp);
}

void createContacts(){
    FILE *fp;
    fp = fopen("contacts.txt", "w+");
    fprintf(fp, "|\n");
    fclose(fp);
}

void createUsers(){
    FILE *fp;
    fp = fopen("test.txt", "w+");
    fprintf(fp, "user password\n");
    fclose(fp);
}

void createMessages(){
    FILE *fp;
    fp = fopen("messages.txt", "w+");
    fprintf(fp, "|\n");
    fclose(fp);
}

void registerContacts(){
    FILE *fp;
    fp = fopen("contacts.txt", "a");
    fprintf(fp, "|\n");
    fclose(fp);
}

int getLinesCount(char* file){
    FILE *fp;
    char ch;
    int linesCount=0;
    fp = fopen(file, "r");
    while((ch=fgetc(fp))!=EOF) {
        if(ch=='\n')
            linesCount++;
    }
    fclose(fp);

    return linesCount;
}

void readFileUsers(char* users[]) {
    FILE *fp1;

    char buff[255];
    int index = 0;

    fp1 = fopen("test.txt", "r");
    while(fgets(buff, 255, fp1)){
        int length = strlen(buff);
        users[index] = (char*)malloc((length+1) * sizeof(char));
        memcpy(users[index], buff, length+1);
        index++;
    }
    fclose(fp1);
}

void readFileContacts(char* contacts[]) {
    FILE *fp2;

    char buff[255];
    int index = 0;

    fp2 = fopen("contacts.txt", "r");
    while(fgets(buff, 255, fp2)){
        int length = strlen(buff);
        contacts[index] = (char*)malloc((length+1) * sizeof(char));
        memcpy(contacts[index], buff, length+1);
        index++;
    }
    fclose(fp2);
}

void readFileMessages(char* messages[]) {
    FILE *fp3;

    char buff[255];
    int index = 0;

    fp3 = fopen("contacts.txt", "r");
    while(fgets(buff, 255, fp3)){
        int length = strlen(buff);
        messages[index] = (char*)malloc((length+1) * sizeof(char));
        memcpy(messages[index], buff, length+1);
        index++;
    }
    fclose(fp3);
}

void rewriteFile(char* array[], int size, char* path) {
    FILE *fp;
    fp = fopen(path, "w+");
    for (int i = 0; i < size; ++i) {
        if (array[i] != NULL) {
            fprintf(fp, "%s", array[i]);
        }
    }
    fclose(fp);
}

void writeMessage(char* message){
    FILE *fp;
    fp = fopen("messages.txt", "a");
    fprintf(fp, "%s", message);
    fclose(fp);
}

