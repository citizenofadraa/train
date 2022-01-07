//
// Created by Martin on 5. 1. 2022.
//

#include "switchManager.h"

int switchClient(int n, int sockfd, char* buffer) {

    printf("What do you want to do? (type the number next to your choice)\n");
    printf("1 Delete account\n");
    printf("2 Send message\n");
    printf("3 Send file\n");
    printf("4 Add contact\n");
    printf("5 Delete contact\n");
    printf("6 Start group conversation\n");
    printf("7 Read message\n");
    printf("8 Read request for contact\n");
    printf("9 Log out\n");

    bzero(buffer,256);
    fgets(buffer, 255, stdin);
    char* ptr;

    switch (strtol(buffer, &ptr, 10)) {
        case 1:
        {
            writeSocketClient(n, sockfd, "1");
            break;
        }
        case 2:
        {
            writeSocketClient(n, sockfd, "2");
            printf("Enter login of target: ");
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            writeSocketClient(n, sockfd, buffer);

            readSocketClient(n, sockfd, buffer);
            if (strcmp(buffer,"not success") != 0){
                printf("Enter message: ");
                bzero(buffer,256);
                fgets(buffer, 255, stdin);
                writeSocketClient(n, sockfd, buffer);
                break;
            } else {
                printf("This login does not exists \n");
                return 7;
            }
        }
        case 3:
        {
            break;
        }
        case 4:
        {
            writeSocketClient(n, sockfd, "4");
            readSocketClient(n,sockfd,buffer);
            printf("All users: %s\n",buffer);
            printf("Select user to join: \n");
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            writeSocketClient(n, sockfd, buffer);
            break;
        }
        case 5:
        {
            writeSocketClient(n, sockfd, "5");
            readSocketClient(n, sockfd, buffer);
            printf("Your contacts: %s", buffer);
            break;
        }
        case 6:
        {
            break;
        }
        case 7:
        {
            writeSocketClient(n, sockfd, "7");
            readSocketClient(n, sockfd, buffer);
            printf("%s", buffer);
            break;
        }
        case 8:
        {
            writeSocketClient(n, sockfd, "8");
            readSocketClient(n,sockfd, buffer);
            char* sender = strtok(strdup(buffer), "\n");
            printf("%s wants to add you as contact, accept? (y|n)\n", sender);
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            strcat(buffer, sender);
            writeSocketClient(n, sockfd, buffer);
            printf("%s", buffer);

            /*readSocketClient(n, sockfd, buffer);
            if (strcmp(buffer,"not success") != 0) {
                writeSocketClient(n, sockfd, sender);
            }
            char* sender = strtok(buffer, "|");
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            strcat(buffer,"|");
            strcat(buffer,sender);
            writeSocketClient(n, sockfd, buffer);*/
        }
        case 9:
        {
            writeSocketClient(n, sockfd, "9");
            break;
        }
        default:
            break;
    }
}