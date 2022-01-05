//
// Created by Adraa on 3. 1. 2022.
//

#include "fileManager.h"

FILE* fp;

void writeToFile(char* tokenLogin, char* tokenPassword) {
    fp = fopen("test.txt", "a");
    fprintf(fp, "%s %s\n", tokenLogin, tokenPassword);
    fclose(fp);
}

void writeToFileContact(char* contacts[], int size) {
    fp = fopen("contacts.txt", "w+");
    for (int i = 0; i < size; ++i) {
        fprintf(fp, "%s\n", contacts[i]);
    }
    fclose(fp);
}

int getLinesCount(){
    char ch;
    int linesCount=0;
    fp = fopen("test.txt", "r");
    while((ch=fgetc(fp))!=EOF) {
        if(ch=='\n')
            linesCount++;
    }
    fclose(fp);

    return linesCount;
}

void readFile(char* users[]) {

    char buff[255];
    int index = 0;

    fp = fopen("test.txt", "r");
    while(fgets(buff, 255, fp)){
        int length = strlen(buff);
        users[index] = (char*)malloc((length+1) * sizeof(char));
        memcpy(users[index], buff, length+1);
        index++;
    }
    fclose(fp);
}

void rewriteFile(char* users[], int size) {
    fp = fopen("test.txt", "w+");
    for (int i = 0; i < size; ++i) {
        if (users[i] != NULL) {
            fprintf(fp, "%s", users[i]);
        }
    }
    fclose(fp);
}

