#include"sock_create.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

int sock_create(char *argv, int fl)
{
    int s;

    if(!strcmp(argv, "UDP"))
    {
        if((s=socket(AF_INET, SOCK_DGRAM, 0))==-1)
        {
            fprintf(stderr, "\n[-]Error in creating udp socket: %s\n", strerror(errno));
        }
    }
    else
    {
        struct sockaddr_in addr;
        addr.sin_family=AF_INET;
        addr.sin_addr.s_addr=inet_addr(strtok(argv, ":"));

        if((s=socket(AF_INET, SOCK_STREAM, 0))==-1)
        {
            fprintf(stderr, "\n[-]Error in creating tcp socket for %s (flag:%d): %s\n", inet_ntoa(addr.sin_addr), fl, strerror(errno));
        }

        if(fl)//client
        {
            addr.sin_port=htons(12346);
            if(connect(s, (struct sockaddr *)&addr, sizeof(addr))==-1)
            {
                fprintf(stderr, "\n[-]Error in connecting to %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
                s=-1;
                goto exit;
            }

            printf("\n[!]Successfully connected to relay at: %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
        }
        else
        {
            addr.sin_port=htons(12345);

            if(bind(s, (struct sockaddr *)&addr, sizeof(addr))==-1)
            {
                fprintf(stderr, "\n[-]Error in binding sock at: %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
                s=-1;
                goto exit;
            }

            if(listen(s, 5)==-1)
            {
                fprintf(stderr, "\n[-]Error in  listning at %s:%d: %s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), strerror(errno));
                s=-1;
                goto exit;
            }

            printf("\n[!]Tcp server successfully bound and listning\n");
        }
    }

    exit:
    return s;
}

