//
// Created by Adraa on 3. 1. 2022.
//

#include "registrationManager.h"

int registerUserServer(char* buffer, int n, int newsockfd, int size, char* users[]) {

    char* tokenLogin;
    char* tokenPassword;
    char* msg;

    readSocketServer(buffer, n, newsockfd);
    char* haveAccount = strtok(buffer, "\n");
    printf("%s", haveAccount);
    if (strcmp(haveAccount,"n")==0){
        readSocketServer(buffer, n, newsockfd);
        char* create = strtok(buffer, "\n");
        printf("%s", buffer);
        if (strcmp(create,"n")==0){
            return 9;
        } else {
            bool existsLogin = false;

            readSocketServer(buffer, n, newsockfd);
            tokenLogin = strtok(strdup(buffer), "\n");

            for (int i = 0; i < size; ++i) {
                int length = strlen(users[i]);
                char *user = (char*)malloc((length+1) * sizeof(char));
                memcpy(user, users[i], length+1);
                char* login = strtok(user, " ");

                if (strcmp(login, tokenLogin) == 0){
                    existsLogin = true;
                    break;
                }
            }

            if (existsLogin == true){
                msg = "not success";
                writeSocketServer(n, newsockfd, msg);
                return 11;
            } else{
                msg = "success";
                writeSocketServer(n, newsockfd, msg);
            }

            readSocketServer(buffer, n, newsockfd);
            tokenPassword = strtok(buffer, "\n");

            writeToFile(tokenLogin, tokenPassword);

        }
    }
    printf("%s", buffer);
}

int registerUserClient(char* buffer, int n, int sockfd) {

    printf("Do you have an account(y|n)? \n");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);
    char* answer = strtok(buffer, "\n");

    if(strcmp(answer,"n") == 0){
        writeSocketClient(n, sockfd, buffer);
        printf("Do you want to register?(y|n) \n");
        bzero(buffer,256);
        fgets(buffer, 255, stdin);
        answer = strtok(buffer, "\n");

        if (strcmp(buffer,"n") == 0){
            writeSocketClient(n, sockfd, buffer);
            return 9;
        } else{
            writeSocketClient(n, sockfd, buffer);
            printf("Enter login: ");
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            writeSocketClient(n, sockfd, buffer);

            readSocketClient(n, sockfd, buffer);
            if (strcmp(buffer,"not success") == 0){
                printf("This login already exists \n");
                return 11;
            }

            printf("Enter login: ");
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            writeSocketClient(n, sockfd, buffer);
        }
    } else {
        writeSocketClient(n, sockfd, buffer);
    }
}