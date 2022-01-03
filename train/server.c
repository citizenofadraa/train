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
    printf("Here is the message: %s\n", buffer);
    return 0;
}

int writeSocket(int n, int newsockfd){
    const char* msg = "I got your message";
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

void readFile(char* users, int size){
    FILE *fp;
    char buff[255];

    int index = 0;
    char* usrs[size];

    fp = fopen("C:\\Users\\Martin\\CLionProjects\\train\\test.txt", "r");
    while(fgets(buff, 255, (FILE*)fp) != NULL){
        char* user = buff;
        usrs[index] = strdup(user);
        index++;
    }
    fclose(fp);

    for (int i = 0; i < size; ++i) {
        users[i] = usrs[i];
    }
}

int main(int argc, char *argv[])
{
    int size = getLinesCount();
    char* users[size+2];
    readFile(users, size);

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

    int status = readSocket(buffer, n, newsockfd);
    bool exists = false;
    if (status == 0){
        for (int i = 0; i < sizeof(users)/sizeof(char) * 2; ++i) {
            char* user = users[i];
            char * token = strtok(user, " ");

            if (i%2 == 0){
                if (buffer == token){
                    exists = true;
                    break;
                }
            }
            while( token != NULL ) {
                token = strtok(NULL, " ");
            }
        }
    }

    writeSocket(n, newsockfd);

    close(newsockfd);
    close(sockfd);

    return 0;
}

