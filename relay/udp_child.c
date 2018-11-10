#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<signal.h>

#include"global_defs.h"
#include"udp_child.h"
#include"snd_rcv.h"
#include"list.h"
#include"allocate.h"

union list *new;

void udp_child()
{
    start_nn=NULL;
    new=(union list *)allocate("union list", 1);
    new->nxt=NULL;
    new->prev=NULL;
    new->nn.msg=(char *)allocate("char", 50);
    int stat, num=1;
    a=&num;
    char *cmdr=(char *)allocate("char", 512), *addr=(char *)allocate("char", 50);
    sprintf(cmdr, "genisis");
    sprintf(addr, "genisis");

    //signal
    struct sigaction action;
    explicit_bzero(&action, sizeof(struct sigaction));
    action.sa_handler=termination;
    sigaction(SIGTERM, &action, NULL);
    
    for(int i=0; *a;)
    {
        free(cmdr);
        free(addr);

        char *addr=(char *)allocate("char", 50);
        if((cmdr=rcv_frm(addr))==NULL)
        {
            fprintf(stderr, "\n[-]Error in receving: %d\n", i);
            break;
        }
        sprintf(new->nn.msg, "%s", cmdr);
        printf("\n[!]Received %s from %s\n", new->nn.msg, addr);
        add_node(new, start_nn, 2);

        explicit_bzero(new->nn.msg, sizeof(char)*50);
        i++;
    }

}

void termination(int sig)
{
    printf("\n[!]Caught SIGNAL: %d. Exiting...\n", sig);
    *a=0;
    free(new);
}
