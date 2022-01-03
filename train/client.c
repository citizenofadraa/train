#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int writeSocket(int n, int sockfd, char* buffer){
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }
    return 0;
}

int readSocket(int n, int sockfd, char* buffer){
    bzero(buffer,256);
    n = read(sockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 6;
    }
}

int main(int argc, char *argv[])
{
    int sockfd, n;
    struct sockaddr_in serv_addr;
    struct hostent* server;

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

    printf("Do you have account(y|n)? \n");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);
    char* answer = strtok(buffer, "\n");

    if(strcmp(answer,"n") == 0){
        writeSocket(n, sockfd, buffer);
        printf("Do you want register?(y|n) \n");
        bzero(buffer,256);
        fgets(buffer, 255, stdin);
        answer = strtok(buffer, "\n");
        if(strcmp(buffer,"n") == 0){
            writeSocket(n, sockfd, buffer);
            return 9;
        } else{
            writeSocket(n, sockfd, buffer);
            readSocket(n, sockfd, buffer);
            printf("%s \n",buffer);
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            writeSocket(n, sockfd, buffer);

            readSocket(n, sockfd, buffer);
            if (strcmp(buffer,"not success") == 0){
                printf("This login already exists \n");
                return 11;
            }

            readSocket(n, sockfd, buffer);
            printf("%s\n",buffer);
            bzero(buffer,256);
            fgets(buffer, 255, stdin);
            writeSocket(n, sockfd, buffer);
        }
    } else{
        writeSocket(n, sockfd, buffer);
    }


    readSocket(n, sockfd, buffer);
    printf("%s\n",buffer);
    bzero(buffer,256);
    fgets(buffer, 255, stdin);
    answer = strtok(buffer, "\n");

    if (strcmp(answer,"n")==0){
        writeSocket(n, sockfd, buffer);
        return 10;
    } else{
        writeSocket(n, sockfd, buffer);
    }

    printf("Enter login: \n");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);

    writeSocket(n, sockfd, buffer);
    readSocket(n, sockfd, buffer);

    if (strcmp(buffer,"not success") != 0){
        printf("Login is OK \n");
    } else {
        printf("This login does not exists \n");
        return 7;
    }

    printf("Please enter a password: ");
    bzero(buffer,256);
    fgets(buffer, 255, stdin);

    writeSocket(n, sockfd, buffer);
    readSocket(n, sockfd, buffer);
    if (strcmp(buffer,"not success") != 0){
        printf("Password is OK, you are signed in \n");
    } else {
        printf("This password is not good with login \n");
        return 8;
    }

    close(sockfd);

    return 0;
}
