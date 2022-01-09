#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "registrationManager.h"
#include "signingManager.h"
#include "switchManager.h"

int main(int argc, char *argv[])
{

    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

    char* answer = "";
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

    int status = signInClient(buffer, n, sockfd);

    readSocketClient(n, sockfd, buffer);
    printf("Your messages:\n");
    printf("%s",buffer);

    if (status == 0) {
        printf("What do you want to do? (type the number next to your choice)\n");
        printf("1 Delete account\n");
        printf("2 Send message\n");
        printf("3 Add contact\n");
        printf("4 Delete contact\n");
        printf("5 Start group conversation\n");
        printf("6 Read message\n");
        printf("7 Read request for contact\n");
        printf("9 Log out\n");

        bzero(buffer,256);
        fgets(buffer, 255, stdin);
        char* ptr;

        switch (strtol(buffer, &ptr, 10)) {
            case 1:
            {

                writeSocketClient(n, sockfd, "1");
                readSocketClient(n, sockfd, buffer);
                if (strcmp(buffer, "no") == 0){
                    printf("Cannot delete account while you have some contacts!");
                }
                break;
            }
            case 2:
            {
                writeSocketClient(n, sockfd, "2");
                readSocketClient(n, sockfd, buffer);
                printf("Your contacts: %s", buffer);

                while (strcmp(answer, "0") != 0) {
                    printf("Enter login of target (to end type 0): ");
                    bzero(buffer,256);
                    fgets(buffer, 255, stdin);
                    answer = strtok(strdup(buffer), "\n");
                    writeSocketClient(n, sockfd, buffer);
                    readSocketClient(n, sockfd, buffer);

                    if (strcmp(buffer,"not success") == 0) {
                        printf("You cannot send message to this user!\n");
                    }
                }

                printf("Enter message: ");
                bzero(buffer,256);
                fgets(buffer, 255, stdin);
                writeSocketClient(n, sockfd, buffer);

                readSocketClient(n, sockfd, buffer);
                printf("Send a ciphered message? (y|n)\n");
                bzero(buffer,256);
                fgets(buffer, 255, stdin);
                writeSocketClient(n, sockfd, buffer);
                break;

            }
            case 3:
            {
                writeSocketClient(n, sockfd, "4");
                readSocketClient(n,sockfd,buffer);
                printf("All online users: %s\n",buffer);
                /*printf(chcete pridat (y|n))
                if yes {

                }*/
                printf("Select user to join: \n");
                bzero(buffer,256);
                fgets(buffer, 255, stdin);
                writeSocketClient(n, sockfd, buffer);
                break;
            }
            case 4:
            {
                writeSocketClient(n, sockfd, "5");
                readSocketClient(n, sockfd, buffer);
                printf("Your contacts: %s", buffer);
                printf("Enter contact to delete: ");
                bzero(buffer,256);
                fgets(buffer, 255, stdin);
                writeSocketClient(n, sockfd, buffer);
                break;
            }
            case 5:
            {
                writeSocketClient(n, sockfd, "6");
                readSocketClient(n, sockfd, buffer);
                printf("Your contacts: %s", buffer);

                printf("Enter login of target (to end type 0): ");
                bzero(buffer,256);
                fgets(buffer, 255, stdin);
                writeSocketClient(n, sockfd, buffer);

                printf("Input file name: \n");
                bzero(buffer,256);
                fgets(buffer, 255, stdin);
                FILE *fp1;
                fp1 = fopen(strtok(buffer, "\n"), "r");
                if (fp1 != NULL) {
                    sendFile(fp1, sockfd, n);
                }
                fclose(fp1);
                break;
            }
            case 6:
            {
                writeSocketClient(n, sockfd, "7");
                readSocketClient(n, sockfd, buffer);
                printf("Your messages:\n");
                printf("%s",buffer);
                break;
            }
            case 7:
            {
                writeSocketClient(n, sockfd, "8");
                readSocketClient(n,sockfd, buffer);
                if (strcmp(buffer,"noone") != 0){
                    char* sender = strtok(strdup(buffer), "\n");
                    printf("%s wants to add you as contact, accept? (y|n)\n", sender);
                    bzero(buffer,256);
                    fgets(buffer, 255, stdin);
                    strcat(buffer, sender);
                    strcat(buffer, "\n");
                    writeSocketClient(n, sockfd, buffer);
                    printf("%s", buffer);
                } else{
                    printf("No requests!\n");
                    char* msg = "no\n";
                    writeSocketClient(n, sockfd, msg);
                }
                break;
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

    close(sockfd);

    return 0;
}
