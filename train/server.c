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

int getTargetIndex(char* target, int size, char *users[]){
    int targetIndex = -1;
    for (int i = 0; i < size; ++i) {
        char* login = getLoginByIndex(users[i]);

        if ((strcmp(login, target) == 0) ){
            targetIndex = i;
        }
    }

    return targetIndex;
}

void* clientCommunication(void *data) {

    D *d = data;

    char* messages[256];
    int messageIndex = 0;

    int newsockfd = 0;
    char buffer[256];

    int size = getLinesCount("test.txt");
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



    readFile(users, "test.txt");
    readFile(contacts, "contacts.txt");

    registerUserServer(buffer, n, newsockfd, size, users);

    readFile(users, "test.txt");
    readFile(contacts, "contacts.txt");

    size = getLinesCount("test.txt");

    int index;
    index = signInServer(buffer, n, newsockfd, size, users);

    if (index > -1) {

        d->descriptors[index] = newsockfd;

        char* myLogin = getLoginByIndex(users[index]);
        char* answer;
        char* ptr;

        readSocketServer(buffer, n, newsockfd);
        int messagesSize = getLinesCount("messages.txt");
        readFile(messages, "messages.txt");
        char myMessages[10000];
        for (int i = 0; i < messagesSize-1; ++i) {
            char fullMsg[255];
            strcpy(fullMsg, messages[i]);
            char* reciever = strtok(fullMsg,"|");
            char* messag = strtok(NULL, "\n");
            if (strcmp(reciever, myLogin) == 0){
                strcat(myMessages,messag);
                strcat(myMessages, "\n");
                messages[i] = NULL;
            }
        }

        printf("%s", myMessages);
        writeSocketServer(n,newsockfd, myMessages);

        rewriteFile(messages, messagesSize, "messages.txt");


        readSocketServer(buffer, n, newsockfd);
        answer = strtok(buffer, "\n");

        int myConSize = 0;
        char *myContacts[150];
        char con[255];
        strcpy(con, contacts[index]);
        char* contact = strtok(con, "|");
        while (contact != NULL){
            myContacts[myConSize] = contact;
            myConSize++;
            contact = strtok(NULL, "|");
        }
        myContacts[myConSize-1] = NULL;

        switch (strtol(answer, &ptr, 10)) {
            case 1:
            {
                if (myConSize > 0){
                    writeSocketServer(n,newsockfd, "no");
                    break;
                } else {
                    writeSocketServer(n,newsockfd, "yes");
                }
                users[index] = NULL;
                rewriteFile(users, size, "test.txt");
                readFile(users, "test.txt");
                contacts[index] = NULL;
                rewriteFile(contacts, size, "contacts.txt");
                readFile(contacts, "contacts.txt");
                break;
            }
            case 2:
            {
                char msg[150];
                strcpy(msg, contacts[index]);
                writeSocketServer(n, newsockfd, msg);
                int targetIndex = -1;
                readSocketServer(buffer, n, newsockfd);
                char* target = strtok(buffer, "\n");

                for (int i = 0; i < myConSize-1; ++i) {
                    char* login = getLoginByIndex(myContacts[i]);

                    if (strcmp(login, target) == 0){
                        targetIndex = getTargetIndex(target,size,users);
                    }
                }

                if (targetIndex < 0){
                    char* msg = "not success";
                    writeSocketServer(n, newsockfd, msg);
                } else {
                    char message[1000];
                    strcat(message,target);
                    char* msg = "success";
                    writeSocketServer(n, newsockfd, msg);
                    readSocketServer(buffer, n, newsockfd);
                    strcat(message, "|");
                    strcat(message,myLogin);
                    strcat(message, ": ");
                    strcat(message, buffer);
                    writeMessage(message);
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
                    if (index != i && d->descriptors[i] > 0 ) {
                        char* login = getLoginByIndex(users[i]);
                        strcat(usrs,login);
                        strcat(usrs," ");
                    }
                }
                writeSocketServer(n,newsockfd, usrs);

                readSocketServer(buffer,n,newsockfd);
                char* target = strtok(buffer, "\n");
                int targetIndex = -1;

                for (int i = 0; i < size; ++i) {
                    char* login = getLoginByIndex(users[i]);

                    if ((strcmp(login, target) == 0) ){
                        targetIndex = i;
                    }
                }

                bool isContact = false;
                for (int i = 0; i < myConSize-1; ++i) {
                    if (strcmp(target, myContacts[i]) == 0) {
                        isContact = true;
                    }
                }

                if (d->descriptors[targetIndex] != 0 && isContact == false){
                    char msg[256];
                    writeSocketServer(n, d->descriptors[targetIndex], myLogin);
                } else{
                    writeSocketServer(n, d->descriptors[targetIndex], "noone");
                }
                break;
            }
            case 5:
            {
                char msg[150];
                strcpy(msg, contacts[index]);
                writeSocketServer(n, newsockfd, msg);
                readSocketServer(buffer, n, newsockfd);
                char *deleted = strtok(buffer, "\n");
                char new[150] = "|";
                for (int i = 0; i < myConSize-1; ++i) {
                    if (strcmp(deleted,myContacts[i]) != 0){
                        strcat(new,myContacts[i]);
                        strcat(new, "|");
                    }
                }
                strcat(new,"\n");
                contacts[index] = new;

                int targetIndex = getTargetIndex(deleted, size, users);
                char targetContacts[150];
                strcpy(targetContacts, contacts[targetIndex]);
                char newTargets[150];
                strcat(newTargets,"|");

                char* contact = strtok(targetContacts, "|");
                while (contact != NULL){
                    if (strcmp(myLogin, contact) != 0 && strcmp(contact,"\n") != 0){
                        strcat(newTargets,contact);
                        strcat(newTargets, "|");
                        contact = strtok(NULL, "|");
                    } else{
                        contact = strtok(NULL, "|");
                    }
                }
                strcat(newTargets, "\n");
                contacts[targetIndex] = newTargets;

                writeToFileContact(contacts, size);
                readFile(contacts, "contacts.txt");
                myConSize = 0;
                char con[255];
                strcpy(con, contacts[index]);
                char* contac = strtok(con, "|");
                while (contac != NULL){
                    myContacts[myConSize] = contac;
                    myConSize++;
                    contac = strtok(NULL, "|");
                }
                myContacts[myConSize-1] = NULL;
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
                printf("%s", buffer);

                char* confirm = strtok(buffer, "\n");
                char* sender = strtok(NULL, "\n");
                char append[255];
                char append2[255];

                if (confirm == NULL) {
                    break;
                }

                if (strcmp(confirm, "y") == 0) {

                    int indexTarget = -1;
                    for (int i = 0; i < (size); ++i) {
                        int length = strlen(users[i]);
                        char *user = (char*)malloc((length+1) * sizeof(char));
                        memcpy(user, users[i], length+1);
                        char* senderLogin = strtok(user, " ");

                        if (strcmp(sender, senderLogin) == 0){
                            indexTarget = i;
                            break;
                        }
                    }

                    strcat(append, "|");
                    strcat(append, sender);
                    strcat(append, contacts[index]);
                    contacts[index] = strdup(append);

                    strcat(append2, "|");
                    strcat(append2, myLogin);
                    strcat(append2, contacts[indexTarget]);
                    contacts[indexTarget] = strdup(append2);

                    writeToFileContact(contacts, size);
                    readFile(contacts, "contacts.txt");
                    myConSize = 0;
                    char con[255];
                    strcpy(con, contacts[index]);
                    char* contact = strtok(con, "|");
                    while (contact != NULL){
                        myContacts[myConSize] = contact;
                        myConSize++;
                        contact = strtok(NULL, "|");
                    }
                    myContacts[myConSize-1] = NULL;
                }
                break;
            }
            case 9:
            {
                d->descriptors[index] = 0;
                break;
            }
            default:
                break;
        }

    }

    close(newsockfd);

    return 0;
}

int main(int argc, char *argv[])
{
    /*createContacts();
    createUsers();
    createMessages();
    return 0;*/

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

    int size = getLinesCount("test.txt");

    pthread_t threadsApp[size + 1];

    int descriptors[10];
    descriptors[0] = 0;

    D data = {sockfd, *descriptors};

    for (int i = 0; i < size + 1; ++i) {
        pthread_create(&threadsApp[i], NULL, &clientCommunication, &data);
    }

    for (int i = 0; i < size + 1; ++i) {
        pthread_join(threadsApp[i], NULL);
    }

    close(sockfd);
}