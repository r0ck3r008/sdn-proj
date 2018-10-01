#include"sock_create.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

int sock_create(char *argv, int server)
{
    int s;
    struct sockaddr_in addr;

    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(strtok(argv, ":"));
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));
   
    if((s=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in creating sock: %s", strerror(errno));
        return -1;
    }

    if(server)
    {
        if(bind(s, (struct sockaddr *)&addr, sizeof(addr))==-1)
        {
            fprintf(stderr, "\n[-]Error in binding the server socket: %s\n", strerror(errno));
            return -1;
        }

        if(listen(s, 5)==-1)
        {
            fprintf(stderr, "\n[-]Error in listning: %s\n", strerror(errno));
            return -1;
        }

        printf("\n[!]Server socker bound and listning successfully at %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }
    else
    {
        addr.sin_port=htons(12345);
        if(connect(s, (struct sockaddr *)&addr, sizeof(addr))==-1)
        {
            fprintf(stderr, "\n[-]Error in connecting to %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
            return -1;
        }
        printf("\n[!]Connected to %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
    }

    return s;
}

