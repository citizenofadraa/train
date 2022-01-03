#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "registrationManager.h"
#include "signingManager.h"


int main(int argc, char *argv[])
{
    int size = getLinesCount();
    char *users[size + 1];

    int sockfd, newsockfd;
    socklen_t cli_len;
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    char buffer[256];

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
    cli_len = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr*)&cli_addr, &cli_len);
    if (newsockfd < 0)
    {
        perror("ERROR on accept");
        return 3;
    }

    readFile(users);

    registerUserServer(buffer, n, newsockfd, size, users);

    readFile(users);

    size = getLinesCount();

    int index;
    index = signInServer(buffer, n, newsockfd, size, users);

    //char* user = strdup(users[index]);
    //char* login = strtok(user, " ");

    //printf("%s %s", user, login);
    char* answer;
    char* ptr;

    readSocketServer(buffer, n, newsockfd);
    answer = strtok(buffer, "\n");

    printf("%s\n", answer);

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
            break;
        }
        case 3:
        {
            break;
        }
        case 4:
        {
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
        case 9:
        {
            break;
        }
        default:
            break;
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}