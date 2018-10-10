#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include"global_defs.h"
#include"udp_child.h"
#include"snd_rcv.h"
#include"list.h"
#include"allocate.h"

int udp_child()
{
    start_nn=NULL;
    union list *new=(union list *)allocate("union list", 1);
    new->nxt=NULL;
    new->prev=NULL;
    new->nn.msg=(char *)allocate("char", 50);
    int ret=0, stat;
    char *cmdr=(char *)allocate("char", 512), *addr=(char *)allocate("char", 50);
    sprintf(cmdr, "genisis");
    sprintf(addr, "genisis");
    
    for(int i=0; ;)
    {
        free(cmdr);
        free(addr);

        char *addr=(char *)allocate("char", 50);
        if((cmdr=rcv_frm(new->nn.msg))==NULL)
        {
            fprintf(stderr, "\n[-]Error in receving: %d\n", i);
            ret=1;
            break;
        }
        add_node(new, start_nn, 2);

        explicit_bzero(new->nn.msg, sizeof(char)*50);
        i++;
    }

    free(new);
    return ret;
}
