#include"client_run.h"
#include"allocate.h"
#include"snd_rcv.h"

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>

void client_run(int sock)
{
    char *cmdr, *retval=(char *)allocate("char", 256);

    for(;;)
    {
        if((cmdr=rcv(sock, "receive bcast from relay", retval))==NULL)
        {
            fprintf(stderr, "\n[-]%s\n", retval);
            break;
        }
    }
}
