#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#include"global_defs.h"
#include"snd_rcv.h"
#include"allocate.h"

int snd(struct controller *cli, char *cmds, char *reason, char *retval, int sock, int free_it)
{
    if(send(sock, cmds, sizeof(char)*512, 0)==-1)
    {
        if(retval!=NULL)
        {
            explicit_bzero(retval, sizeof(char)*256);
            sprintf(retval, "\n[-]Error in sendig %s to %s:%d for %s: %s\n", cmds, inet_ntoa(cli->addr.sin_addr), ntohs(cli->addr.sin_port), reason, strerror(errno));
        }
        return 1;
    }
    
    if(free_it)
    {
        free(cmds);
    }
    return 0;
}

char *rcv(struct controller *cli, int sock, char *reason, char *retval)
{
    char *cmdr=(char *)allocate("char", 512);

    if(recv(sock, cmdr, sizeof(char)*512, 0)==-1)
    {
        explicit_bzero(retval, sizeof(char)*256);
        sprintf(retval, "\n[-]Error in receving from %s:%d for %s:%s\n", inet_ntoa(cli->addr.sin_addr), sock, reason, strerror(errno));
        return NULL;
    }

    return cmdr;
}

char *rcv_frm(char *addr)
{
    char *cmdr=(char *)allocate("char", 512);
    struct sockaddr_in addr_in;
    socklen_t len=sizeof(addr_in);

    if(recvfrom(udp_sock, cmdr, sizeof(char)*512, 0, (struct sockaddr *)&addr_in, &len)==-1)
    {
        fprintf(stderr, "\n[-]Error in receving:%s\n", strerror(errno));
        return NULL;
    }

    sprintf(addr, "%s:%d", inet_ntoa(addr_in.sin_addr), ntohs(addr_in.sin_port));
    return cmdr;
}
