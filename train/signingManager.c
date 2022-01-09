//
// Created by Adraa on 3. 1. 2022.
//

#include "signingManager.h"

int signInClient(char* buffer, int n, int sockfd) {

    printf("Do you want to sign in?(y|n) \n");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);
    char* answer = strtok(buffer, "\n");

    if (strcmp(answer,"n") == 0){
        writeSocketClient(n, sockfd, buffer);
        return 10;
    } else {
        writeSocketClient(n, sockfd, buffer);
    }

    printf("Enter login: \n");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);

    writeSocketClient(n, sockfd, buffer);
    readSocketClient(n, sockfd, buffer);

    if (strcmp(buffer,"not success") != 0){
        printf("Login is OK \n");
    } else {
        printf("This login does not exists \n");
        return 7;
    }

    printf("Please enter a password: ");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);

    writeSocketClient(n, sockfd, buffer);
    readSocketClient(n, sockfd, buffer);
    if (strcmp(buffer,"not success") != 0){
        printf("Password is OK, you are signed in \n");
        writeSocketClient(n, sockfd, "wait");
    } else {
        printf("This password is not good with login \n");
        return 8;
    }

    return 0;
}

int signInServer(char* buffer, int n, int newsockfd, int size, char* users[]) {

    char* msg;
    char tokenLogin[1000];
    char tokenPassword[1000];
    int index = 0;

    readSocketServer(buffer, n, newsockfd);
    char* signin = strtok(buffer, "\n");
    if (strcmp(signin,"n")==0){
        return 100;
    }


    readSocketServer(buffer, n, newsockfd);
    bool exists = false;
    for (int i = 0; i < (size); ++i) {
        int length = strlen(users[i]);
        char *user = (char*)malloc((length+1) * sizeof(char));
        memcpy(user, users[i], length+1);
        char* tokens = strdup(user);
        strcpy(tokenLogin, strtok(tokens, " "));
        strcpy(tokenPassword, strtok(NULL, " "));
        char* login = strtok(buffer, "\n");

        if (strcmp(buffer, tokenLogin) == 0){
            exists = true;
            index = i;
            free(user);
            free(tokens);
            break;
        }
        free(user);
        free(tokens);
    }

    if (exists == false){
        msg = "not success";
        writeSocketServer(n, newsockfd, msg);
        printf("You have no account with login %s \n", buffer);
        return -2;
    } else{
        msg = "success";
        writeSocketServer(n, newsockfd, msg);
    }

    readSocketServer(buffer,n,newsockfd);
    exists = false;

    strcpy(tokenPassword, strtok(tokenPassword, "\n"));
    char* password = strtok(buffer, "\n");

    if (strcmp(buffer, tokenPassword) == 0){
        exists = true;
    }

    if (exists == false){
        msg = "not success";
        writeSocketServer(n, newsockfd, msg);
        printf("You have not account with password %s \n", password);
        return -3;
    } else{
        msg = "success";
        writeSocketServer(n, newsockfd, msg);
    }

    return index;

}