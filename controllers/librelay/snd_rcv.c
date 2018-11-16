#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

#include"snd_rcv.h"
#include"allocate.h"

int snd(int sock, char *cmds, char *reason, char *retval)
{
    if(send(sock, cmds, sizeof(char)*512, 0)==-1)
    {
        sprintf(retval, "\n[-]Error in sending for %s to sock: %d %s\n", reason, sock, strerror(errno));
        return 1;
    }

    printf("\n[!]Sent: %s\n", cmds);

    free(cmds);
    return 0;
}

char *rcv(int sock, char *reason, char *retval)
{
    char *cmdr=(char *)allocate("char", 512);

    if(recv(sock, cmdr, sizeof(char)*512, 0)==-1)
    {
        sprintf(retval, "\n[-]Error in receving for %s from sock %d: %s\n", reason, sock, strerror(errno));
        return NULL;
    }

    return cmdr;
}

int snd_to(int sock, struct sockaddr_in *addr, char *cmds, char *reason, char *retval)
{
    if(sendto(sock, cmds, sizeof(char)*512, 0, (struct sockaddr *)addr, sizeof(struct sockaddr_in))==-1)
    {
        sprintf(retval, "\n[-]Error in sending %s to %s:%d for %s:%s\n", cmds, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port), reason, strerror(errno));
        return 1;
    }

    free(cmds);
    return 0;
}
