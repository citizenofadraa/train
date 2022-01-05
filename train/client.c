#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "registrationManager.h"
#include "signingManager.h"


int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char* answer;
    char buffer[256];

    if (argc < 3)
    {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        return 1;
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        return 2;
    }

    bzero((char*)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(
            (char*)server->h_addr,
            (char*)&serv_addr.sin_addr.s_addr,
            server->h_length
    );
    serv_addr.sin_port = htons(atoi(argv[2]));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        perror("Error creating socket");
        return 3;
    }

    if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error connecting to socket");
        return 4;
    }

    registerUserClient(buffer, n, sockfd);

    signInClient(buffer, n, sockfd);

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
            readSocketClient(n,sockfd, buffer);
            printf("%s",buffer);
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
            writeSocketClient(n, sockfd, "7");
            readSocketClient(n, sockfd, buffer);
            printf("%s", buffer);
            break;
        }
        case 8:
        {
            readSocketClient(n,sockfd, buffer);
            printf("%s \n", buffer);
            char* sender = strtok(buffer, "|");
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            strcat(buffer,"|");
            strcat(buffer,sender);
            writeSocketClient(n, sockfd, buffer);
        }
        case 9:
        {
            writeSocketClient(n, sockfd, "9");
            break;
        }
        default:
            break;
    }

    close(sockfd);

    return 0;
}
