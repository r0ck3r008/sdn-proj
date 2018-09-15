#define NEEDS_SOCK

#include"sock_create.h"
#include"global_defs.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

int sock_create(char *argv1)
{
    if((sock=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in creating socket: %s\n", strerror(errno));
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(strtok(argv1, ":"));
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));

    if(bind(sock, (struct sockaddr *)&addr, sizeof(addr))==-1)
    {
        fprintf(stderr, "\n[-]Error in binding the socket: %s\n", strerror(errno));
        return 1;
    }
    printf("\n[!]Socker created and bound successfully\n");

    return 0;
}
