#include"snd_rcv.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

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
