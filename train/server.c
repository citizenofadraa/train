#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "registrationManager.h"
#include "signingManager.h"

typedef struct data {
    int sockfd;
    int descriptors[50];
} D;

char* getLoginByIndex(char* user){
    int length = strlen(user);
    char *usr = (char*)malloc((length+1) * sizeof(char));
    memcpy(usr, user, length+1);
    char* login = strtok(user, " ");

    return login;
}

void* clientCommunication(void *data) {

    D *d = data;

    int newsockfd = 0;
    char buffer[256];

    int size = getLinesCount();
    char *users[size + 1];
    char *contacts[size + 1];
    int n;

    int indexSocket = 0;
    int connectionNumber = 1;

    struct sockaddr_in cli_addr;
    socklen_t cli_len;
    cli_len = sizeof(cli_addr);

    newsockfd = accept(d->sockfd, (struct sockaddr*)&cli_addr, &cli_len);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        exit(EXIT_FAILURE);
    }

    readFile(users);

    registerUserServer(buffer, n, newsockfd, size, users);

    readFile(users);

    size = getLinesCount();

    int index;
    index = signInServer(buffer, n, newsockfd, size, users);

    d->descriptors[index] = newsockfd;

    char* myLogin = getLoginByIndex(users[index]);

    //char* user = strdup(users[index]);
    //char* login = strtok(user, " ");

    //printf("%s %s", user, login);
    char* answer;
    char* ptr;

    readSocketServer(buffer, n, newsockfd);
    answer = strtok(buffer, "\n");

    switch (strtol(answer, &ptr, 10)) {
        case 1:
        {
            users[index] = NULL;
            rewriteFile(users, size);
            readFile(users);
            break;
        }
        case 2:
        {
            int targetIndex = -1;
            readSocketServer(buffer, n, newsockfd);
            char* target = strtok(buffer, "\n");

            for (int i = 0; i < size; ++i) {
                char* login = getLoginByIndex(users[i]);

                if (strcmp(login, buffer) == 0){
                    targetIndex = i;
                    printf("%d", targetIndex);
                }
            }

            if (targetIndex < 0){
                char* msg = "not success";
                writeSocketServer(n, newsockfd, msg);
            } else{
                char* msg = "success";
                readSocketServer(buffer, n, newsockfd);
                strcat(buffer,myLogin);
                writeSocketServer(n, d->descriptors[targetIndex], buffer);
            }
            break;
        }
        case 3:
        {
            break;
        }
        case 4:
        {
            char usrs[1000];
            for (int i = 0; i < size; ++i) {
                char* login = getLoginByIndex(users[i]);
                printf("%s", login);
                strcat(usrs,login);
                strcat(usrs," ");
            }
            writeSocketServer(n,newsockfd, usrs);

            readSocketServer(buffer,n,newsockfd);
            char* target = strtok(buffer, "\n");
            int targetIndex = -1;

            for (int i = 0; i < size; ++i) {
                char* login = getLoginByIndex(users[i]);

                if (strcmp(login, buffer) == 0){
                    targetIndex = i;
                }
            }

            if (d->descriptors[targetIndex] != 0){
                char msg[256];
                strcat(msg, targetIndex);
                strcat(msg, "|");
                strcat(msg, myLogin);
                strcat(msg," wants to be your contact. Accept(y|n) ?");
                writeSocketServer(n,d->descriptors[targetIndex], msg);
            }
            break;
        }
        case 5:
        {
            break;
        }
        case 6:
        {
            break;
        }
        case 7:
        {
            break;
        }
        case 8:
        {
            readSocketServer(buffer,n,newsockfd);
            char* response = strtok(buffer, "|");
            int targetIndex = strtok(NULL, "\n");
            char* target = getLoginByIndex(users[targetIndex]);
            printf("%s",response);
            if (strcmp(response,"y") == 0){
                contacts[index] = strcat(contacts[index],target);
                contacts[targetIndex] = strcat(contacts[targetIndex],myLogin);
                contacts[index] = strcat(contacts[index],"/");
                contacts[targetIndex] = strcat(contacts[targetIndex],"/");
                writeToFileContact(contacts, size);
                char msg[256];
                strcat(msg, target);
                strcat(msg," accept your contact request");
                writeSocketServer(n, newsockfd, msg);
            } else if(strcmp(response, "n") == 0){
                char msg[256];
                strcat(msg, target);
                strcat(msg," did not accept your contact request");
                writeSocketServer(n, newsockfd, msg);
            }
        }
        case 9:
        {
            d->descriptors[index] = 0;
            break;
        }
        default:
            break;
    }

    close(newsockfd);


    return 0;
}

int main(int argc, char *argv[])
{
    int sockfd;

    struct sockaddr_in serv_addr;

    if (argc < 2)
    {
        fprintf(stderr,"usage %s port\n", argv[0]);
        return 1;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 1;
    }

    if (bind(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error binding socket address");
        return 2;
    }

    listen(sockfd, 5);

    int size = getLinesCount();

    pthread_t threadsApp[size + 1];

    /*for (int i = 0; i < size + 1; ++i) {
        ports[i].socketNumber = atoi(argv[1]) + i;
        ports[i].socket = 0;
    }*/

    int descriptors[10];

    D data = {sockfd, *descriptors};

    for (int i = 0; i < size + 1; ++i) {
        pthread_create(&threadsApp[i], NULL, &clientCommunication, &data);
    }

    for (int i = 0; i < size + 1; ++i) {
        pthread_join(threadsApp[i], NULL);
    }

    close(sockfd);
}