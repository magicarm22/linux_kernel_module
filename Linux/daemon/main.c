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
#include "libmylib.h"

#define IOCTL_SET_MSG _IOR(241, 0, char *)

void    create_daemon()
{
    pid_t sid;

    umask(0);
    sid = setsid();
    if (sid < 0) {
            exit(EXIT_FAILURE);
    }
    if ((chdir("/")) < 0) {
            exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}

void connected(int connfd, int dev_fd)
{
    char buff[8000]; 
    int n;
    int ret_val;

    while (1) {
        bzero(buff, 8000);
        read(connfd, buff, sizeof(buff));
        printf("%s\n", buff);
        if (strncmp(buff, "list", 4) == 0)
        {
            bzero(buff, 8000);
            read(dev_fd, buff, sizeof(buff));
            write(connfd, buff, sizeof(buff));
        }
        else if (strncmp(buff, "kill", 4) == 0)
        {
            ret_val = pwrite(dev_fd, &buff, sizeof(buff), 0);
            if (ret_val < 0) {
                bzero(buff, 8000);
                ret_val = snprintf(buff, sizeof(buff), "%s %s\n", strerror(errno), ft_itoa(ret_val));
                write(connfd, buff, ret_val);
            }
            else
                write(connfd, "Kill success!\n", 15);
        }
        else
            write(connfd, "Unknown command\n", sizeof(buff));
    }
}

void daemon_loop()
{
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    time_t ticks;
    int dev_fd;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(5000);

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);

    dev_fd = open("/dev/my_kernel", O_RDWR);

    while(1) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        connected(connfd, dev_fd);
        close(connfd);
     }
}

int main(int argc, char** argv)
{
    int status;
    pid_t pid;

    pid = fork();
    if (pid == -1)
    {
        printf("Error: Start Daemon failed (%s)\n", strerror(errno));
        return -1;
    }
    else if (!pid)
    {
        create_daemon();
        daemon_loop();
        return status;
    }
    else
        return 0;
}