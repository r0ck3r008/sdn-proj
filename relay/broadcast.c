#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<pthread.h>
#include<errno.h>

#include"global_defs.h"
#include"broadcast.h"
#include"list.h"
#include"allocate.h"
#include"snd_rcv.h"

int broadcast(struct controller *sender, char *cmds)
{
    pid_t main_pid=getpid();
    int num_ctrlr=list_len(ctrlr_start);

    if(num_ctrlr==-1)
    {
        fprintf(stderr, "\n[-]Empty list passed to bcast function for controller %s for cmd: %s\n", inet_ntoa(sender->addr.sin_addr), cmds);
        return -1;
    }

    {//limiting the curr to scope of for loop only
        union node *curr=ctrlr_start->nxt;
        for(int i=0; i<num_ctrlr || curr->nxt!=NULL; i++, curr=curr->nxt)
        {
            char *addr=inet_ntoa(curr->ctrlr->addr.sin_addr);
            if(strcmp(addr, inet_ntoa(sender->addr.sin_addr))) //same controller skip
            {
                int tag=curr->tag;
                if(fork()==0)
                {
                    //child
                    int tag_for_child=tag;
                    broadcast_run(find_node(ctrlr_start, tag_for_child), cmds);
                    break;
                }
            }
        }
    }
}

void broadcast_run(union node *recepient, char *cmds)
{
    
}
