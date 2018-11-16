#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#include"tcp_connector.h"
#include"allocate.h"
#include"snd_rcv.h"

int get_connection_back(int sock)
{
    socklen_t len=sizeof(struct sockaddr_in);
    struct sockaddr_in addr;
    int s;
    
    if((s=accept(sock, (struct sockaddr *)&addr, &len))==-1)
    {
        fprintf(stderr, "\n[-]Error in receving back the connection: %s\n", strerror(errno));
        goto exit;
    }

    printf("\n[!]Got a connection back from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

exit:
    return s;
}

int send_to_relay(int sock, int flag, char *addr)
{
    int ret=0;
    char *cmds=(char *)allocate("char", 512), *retval=(char*)allocate("char", 128), *reason=(char *)allocate("char", 64);

    if(flag)
    {
        sprintf(cmds, "BROADCAST:%s", addr);
        sprintf(reason, "to broadcast for %s", addr);
    }
    else
    {
        sprintf(cmds, "%s", addr);
        sprintf(reason, "to reply for %s", addr);
    }

    if(snd(sock, cmds, reason, retval))
    {
        fprintf(stderr, "[-]%s", retval);
        ret=1;
    }

    free(reason);
    free(retval);
    return ret;
}

char *recv_bcast(int sock)
{
    char *cmdr, *retval=(char *)allocate("char", 128);

    if((cmdr=rcv(sock, "receive broadcast if any", retval))==NULL)
    {
        fprintf(stderr, "\n[-]%s", retval);
    }

    free(retval);
    return cmdr;
}
