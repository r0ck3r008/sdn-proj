#define NEEDS_SOCK

#include"global_defs.h"
#include"allocate.h"
#include"snd_rcv.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>

int snd(struct sockaddr_in recepient, char *msg, char *reason)
{
    if(sendto(sock, msg, sizeof(char)*512, 0, (struct sockaddr *)&recepient, sizeof(recepient))==-1)
    {
        fprintf(stderr, "\n[-]Error in sending %s to %s:%d, for reason %s:%s\n", msg, inet_ntoa(recepient.sin_addr), ntohs(recepient.sin_port), reason, strerror(errno));
        return 1;
    }

    free(msg);
    return 0;
}

char *rcv(struct sockaddr_in *from, char *reason)
{
    char *cmdr=(char *)allocate("char", 512*sizeof(char));
    socklen_t len=sizeof(struct sockaddr_in);

    if(recvfrom(sock, cmdr, sizeof(char)*512, 0, (struct sockaddr *)from, &len)==-1)
    {
        fprintf(stderr, "\n[-]Error in receving for reason %s: %s\n", reason, strerror(errno));
        return NULL;
    }

    return cmdr;
}
