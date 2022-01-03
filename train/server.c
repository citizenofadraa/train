#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>


int readSocket(char *buffer, int n, int newsockfd){
    bzero(buffer,256);
    n = read(newsockfd, buffer, 255);
    if (n < 0)
    {
        perror("Error reading from socket");
        return 4;
    }
    return 0;
}

int writeSocket(int n, int newsockfd, char* msg){
    n = write(newsockfd, msg, strlen(msg)+1);
    if (n < 0)
    {
        perror("Error writing to socket");
        return 5;
    }
    return 0;
}

int getLinesCount(){
    FILE *fp;
    char ch;
    int linesCount=0;
    fp = fopen("C:\\Users\\Martin\\CLionProjects\\train\\test.txt", "r");
    while((ch=fgetc(fp))!=EOF) {
        if(ch=='\n')
            linesCount++;
    }
    fclose(fp);

    return linesCount;
}


int main(int argc, char *argv[])
{
    int size = getLinesCount();
    char *users[size];
    FILE *fp;
    char buff[255];

    int index = 0;
    char* tokenLogin;
    char* tokenPassword;
    char* msg;

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

    fp = fopen("C:\\Users\\Martin\\CLionProjects\\train\\test.txt", "r");
    while(fgets(buff, 255, fp)){
        int length = strlen(buff);
        users[index] = (char*)malloc((length+1) * sizeof(char));
        memcpy(users[index], buff, length+1);
        index++;
    }

    readSocket(buffer, n, newsockfd);
    char* haveAccount = strtok(buffer, "\n");
    if (strcmp(haveAccount,"n")==0){
        readSocket(buffer, n, newsockfd);
        char* create = strtok(buffer, "\n");
        if (strcmp(create,"n")==0){
            return 9;
        } else{
            bool existsLogin = false;

            msg = "Enter login: ";
            writeSocket(n, newsockfd, msg);
            readSocket(buffer, n, newsockfd);
            tokenLogin = strtok(buffer, "\n");

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
                writeSocket(n, newsockfd, msg);
                return 11;
            } else{
                msg = "success";
                writeSocket(n, newsockfd, msg);
            }

            msg = "Enter password: ";
            writeSocket(n, newsockfd, msg);
            readSocket(buffer, n, newsockfd);
            tokenPassword = strtok(buffer, "\n");

            fp = fopen("C:\\Users\\Martin\\CLionProjects\\train\\test.txt", "a");
            fprintf(fp, "%s %s\n", tokenLogin, tokenPassword);
            fclose(fp);
        }
    }

    for (int i = 0; i < size; ++i) {
        printf("%s \n", users[i]);
    }

    msg = "Do you want signed in?(y|n) ";
    writeSocket(n, newsockfd, msg);
    readSocket(buffer, n, newsockfd);
    char* signin = strtok(buffer, "\n");
    if (strcmp(signin,"n")==0){
        return 10;
    }

    fp = fopen("C:\\Users\\Martin\\CLionProjects\\train\\test.txt", "r");
    while(fgets(buff, 255, fp)){
        int length = strlen(buff);
        users[index] = (char*)malloc((length+1) * sizeof(char));
        memcpy(users[index], buff, length+1);
        index++;
    }
    fclose(fp);

    readSocket(buffer, n, newsockfd);
    bool exists = false;
    for (int i = 0; i < (size-1); ++i) {
        int length = strlen(users[i]);
        char *user = (char*)malloc((length+1) * sizeof(char));
        memcpy(user, users[i], length+1);
        tokenLogin = strtok(user, " ");
        tokenPassword = strtok(NULL, " ");
        char* login = strtok(buffer, "\n");

        if (strcmp(buffer, tokenLogin) == 0){
            exists = true;
            index = i;
            break;
        }
    }

    if (exists == false){
        msg = "not success";
        writeSocket(n, newsockfd, msg);
        printf("You have no account with login %s \n", buffer);
        return 7;
    } else{
        msg = "success";
        writeSocket(n, newsockfd, msg);
    }

    readSocket(buffer,n,newsockfd);
    exists = false;

    tokenPassword = strtok(tokenPassword, "\n");
    char* password = strtok(buffer, "\n");

    if (strcmp(buffer, tokenPassword) == 0){
        exists = true;
    }

    if (exists == false){
        msg = "not success";
        writeSocket(n, newsockfd, msg);
        printf("You have not account with password %s \n", password);
        return 8;
    } else{
        msg = "success";
        writeSocket(n, newsockfd, msg);
    }

    close(newsockfd);
    close(sockfd);

    return 0;
}

