#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<errno.h>

#include"snd_rcv.h"
#include"allocate.h"

int snd(int fd, char *cmds, char *reason)
{
    if(send(fd, cmds, sizeof(char)*512, 0)==-1)
    {
        fprintf(stderr, "\n[-]Error in sendig %s for %s: %s\n", cmds, reason, strerror(errno));
        return 1;
    }
    
    deallocate(cmds, "char", 512);
    return 0;
}

char *rcv(int fd, char *reason)
{
    char *cmdr=(char *)allocate("char", 512);

    if(recv(fd, cmdr, sizeof(char)*512, 0)==-1)
    {
        fprintf(stderr, "\n[-]Error in receving for %s: %s\n", reason, strerror(errno));
        return NULL;
    }

    return cmdr;
}
