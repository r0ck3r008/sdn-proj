#define NEEDS_SOCK

#include"global_defs.h"
#include"dbconnect.h"
#include"snd_rcv.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<errno.h>

int dbconnect(char *argv2, int disconnect)
{
    char *cmds=(char *)allocate("char", 512), *cmdr;

    if(!disconnect)
    {
        sprintf(cmds, "CONTROLLER UP");
    }
    else
    {
        sprintf(cmds, "CONTROLLER DOWN");
    }

    struct sockaddr_in addr, addr2;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(strtok(argv2, ":"));
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));
    
    if(snd(addr, cmds, "send controller up/down signal"))
    {
        return 1;
    }

    while(1)
    {
        if((cmdr=rcv(&addr2, "receive ack from dbinterface"))==NULL)
        {
            continue;
        }

        if(!strcmp(inet_ntoa(addr2.sin_addr), inet_ntoa(addr.sin_addr))&& !strcmp(cmdr, "OK"))
        {
            free(cmdr);
            continue;
        }
    }
    printf("\n[!]Dbinterface notified correctly\n");

    free(cmdr);
    return 0;
}
