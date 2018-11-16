#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#include"allocate.h"
#include"udp_connector.h"
#include"snd_rcv.h"

int udp_connector(int sock, char *argv)
{
    char *cmds=(char *)allocate("char", 512), *retval=(char *)allocate("char", 128);
    sprintf(cmds, "RSVP");

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(strtok(argv, ":"));
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));

    if(snd_to(sock, &addr, cmds, retval, "to send back rsvp"))
    {
        fprintf(stderr, "%s", retval);
        return 1;
    }

    free(retval);
    return 0;
}
