#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include "registrationManager.h"
#include "signingManager.h"

typedef struct data {
    int sockfd;
    pthread_mutex_t *mutex;
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

void recieveFile(int newsockfd, int n){
    FILE *fp1;
    char *filename = "recv.txt";
    char bufr[1000];

    fp1 = fopen(filename, "w");
    while (1) {
        readSocketServer(bufr, n, newsockfd);
        if (strcmp(bufr, "end") == 0) {
            break;
        }
        bzero(bufr, 1000);
    }
    fclose(fp1);
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

    readFileUsers(users);
    readFileContacts(contacts);

    registerUserServer(buffer, n, newsockfd, size, users);

    readFileUsers(users);
    readFileContacts(contacts);

    size = getLinesCount("test.txt");

    int index;
    index = signInServer(buffer, n, newsockfd, size, users);

    if (index > -1 && index < 100) {

        pthread_mutex_lock(d->mutex);
        d->descriptors[index] = newsockfd;
        pthread_mutex_unlock(d->mutex);

        char* myLogin = getLoginByIndex(users[index]);
        char* answer;
        char* ptr;

        readSocketServer(buffer, n, newsockfd);
        int messagesSize = getLinesCount("messages.txt");
        readFileMessages(messages);
        char myMessages[10000];
        for (int i = 0; i < messagesSize; ++i) {
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
                readFileUsers(users);
                contacts[index] = NULL;
                rewriteFile(contacts, size, "contacts.txt");
                readFileContacts(contacts);
                break;
            }
            case 2:
            {
                char msg[150];
                strcpy(msg, contacts[index]);
                writeSocketServer(n, newsockfd, msg);
                char* trgt = "";
                char* target[256];
                int j = 0;

                while (strcmp(trgt, "0") != 0) {
                    readSocketServer(buffer, n, newsockfd);
                    trgt = strtok(strdup(buffer), "\n");
                    if (strcmp(trgt, "0") != 0) {
                        for (int i = 0; i < myConSize-1; ++i) {
                            char* login = getLoginByIndex(myContacts[i]);

                            if (strcmp(login, trgt) == 0){
                                target[j] = strdup(trgt);
                                j++;
                                writeSocketServer(n, newsockfd, "sucess");
                            } else {
                                writeSocketServer(n, newsockfd, "not sucess");
                            }
                        }
                    }
                }

                writeSocketServer(n, newsockfd, "sucess");

                readSocketServer(buffer, n, newsockfd);

                char buff[256];
                strcpy(buff, buffer);

                writeSocketServer(n, newsockfd, "sucess");
                readSocketServer(buffer, n, newsockfd);

                char *confirm = strtok(buffer, "\n");

                char message[1000];

                for (int i = 0; i < j; ++i) {
                    strcat(message, target[i]);
                    strcat(message, "|");
                    strcat(message, myLogin);
                    strcat(message, ": ");

                    if (strcmp(confirm, "y") == 0) {
                        for (int k = 0; buff[k] != '\n'; ++i) {
                            buff[k] += 3;
                        }
                        strcat(message, strtok(buff, "\n"));
                        strcat(message, " ");
                        strcat(message, "cesarCipher3\n");
                    } else {
                        strcat(message, buff);
                    }
                }

                writeMessage(message);

                break;
            }
            case 3:
            {
                char usrs[1000];
                for (int i = 0; i < size; ++i) {
                    pthread_mutex_lock(d->mutex);
                    if (index != i && d->descriptors[i] > 0 ) {
                        char* login = getLoginByIndex(users[i]);
                        strcat(usrs,login);
                        strcat(usrs," ");
                    }
                    pthread_mutex_unlock(d->mutex);
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

                pthread_mutex_lock(d->mutex);
                if (d->descriptors[targetIndex] != 0 && isContact == false){
                    char msg[256];
                    writeSocketServer(n, d->descriptors[targetIndex], myLogin);
                } else{
                    writeSocketServer(n, d->descriptors[targetIndex], "noone");
                }
                pthread_mutex_unlock(d->mutex);
                break;
            }
            case 4:
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

                contact = strtok(targetContacts, "|");
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
                readFileContacts(contacts);
                myConSize = 0;
                strcpy(con, contacts[index]);
                char* contac = strtok(con, "|");
                while (contac != NULL){
                    myContacts[myConSize] = contac;
                    myConSize++;
                    contac = strtok(NULL, "|");
                }
                myContacts[myConSize-1] = NULL;
                char deletionMsg[256];
                strcat(deletionMsg, deleted);
                strcat(deletionMsg, "|");
                strcat(deletionMsg, myLogin);
                strcat(deletionMsg, " deleted you from his contacts.");
                writeMessage(deletionMsg);
                break;
            }
            case 5:
            {
                char msg[150];
                strcpy(msg, contacts[index]);
                writeSocketServer(n, newsockfd, msg);
                char* trgt = "";
                char* target[256];
                int j = 0;

                while (strcmp(trgt, "0") != 0) {
                    readSocketServer(buffer, n, newsockfd);
                    trgt = strtok(strdup(buffer), "\n");
                    if (strcmp(trgt, "0") != 0) {
                        for (int i = 0; i < myConSize-1; ++i) {
                            char* login = getLoginByIndex(myContacts[i]);

                            if (strcmp(login, trgt) == 0){
                                target[j] = trgt;
                                j++;
                                writeSocketServer(n, newsockfd, "sucess");
                            } else {
                                writeSocketServer(n, newsockfd, "not sucess");
                            }
                        }
                    }
                }

                writeSocketServer(n, newsockfd, "sucess");

                recieveFile(newsockfd, n);

                for (int i = 0; i < j; ++i) {
                    int trgtIndex = getTargetIndex(target[i], size, users);
                    FILE *recv = fopen("recv.txt", "r");
                    if (recv != NULL) {
                        pthread_mutex_lock(d->mutex);
                        sendFile(recv, d->descriptors[trgtIndex], n);
                        pthread_mutex_unlock(d->mutex);
                    }
                    fclose(recv);
                }
                break;
            }
            case 6:
            {
                messagesSize = getLinesCount("messages.txt");
                readFileMessages(messages);
                for (int i = 0; i < messagesSize; ++i) {
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
                break;
            }
            case 7:
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
                            free(user);
                            break;
                        }

                        free(user);
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
                    readFileContacts(contacts);
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
    FILE *test;
    test = fopen("test.txt", "r");
    if (test == NULL) {
        createContacts();
        createUsers();
        createMessages();
    }

    fclose(test);

    int sockfd;

    struct sockaddr_in serv_addr;

    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);

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

    D data = {sockfd, &mutex, *descriptors};

    for (int i = 0; i < size + 1; ++i) {
        pthread_create(&threadsApp[i], NULL, &clientCommunication, &data);
    }

    for (int i = 0; i < size + 1; ++i) {
        pthread_join(threadsApp[i], NULL);
    }

    for (int i = 0; i < 10; ++i) {
        free(&descriptors[i]);
    }

    pthread_mutex_destroy(&mutex);

    close(sockfd);
}