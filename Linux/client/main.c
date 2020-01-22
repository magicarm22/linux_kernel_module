#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/stat.h>

void connected(int sockfd)
{
    char buff[8000]; 
    int n;
    
    n = 0;
    while (1) { 
        bzero(buff, 8000); 
        write(0, "Write command: ", 15);
        while ((buff[n++] = getchar()) != '\n'); 
        write(sockfd, buff, sizeof(buff));
        bzero(buff, sizeof(buff)); 
        sleep(1);
        read(sockfd, buff, sizeof(buff));
        printf("From Server : %s", buff);
        n = 0;
    }
}

int main()
{
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    }

    servaddr.sin_family = AF_INET; 
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
    servaddr.sin_port = htons(5000); 

    
    if (connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != 0) { 
        printf("connection with the server failed...\n"); 
        exit(0);
    }

    connected(sockfd);
}