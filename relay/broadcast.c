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
#include"lock_and_exec.h"
#include"broadcast.h"
#include"list.h"
#include"allocate.h"
#include"snd_rcv.h"

int broadcast(struct controller *sender, char *cmds, struct mutex_call *mcall)
{
    struct mutex_call *mcall2=alloc_mcall(0, 2, mcall->ctrlr_ro, mcall->ctrlr);
    struct func_call *fcall=alloc_fcall(3);
    int list_len=0, ret=0;
    pid_t main_pid=getpid();

    if(lock_and_exec(mcall2, fcall, 2, ctrlr_start, &list_len))
    {
        fprintf(stderr, "\n[-]Error in list_len for %s\n", inet_ntoa(sender->addr.sin_addr));
        ret=1;
        goto exit;
    }

    if(_lock(mcall2))
    {
        fprintf(stderr, "\n[-]Lock failed for %s", inet_ntoa(sender->addr.sin_addr));
        ret=1;
        goto exit;
    }

    union node *curr=ctrlr_start->nxt;
    struct controller *recepient;
    for(int i=0; i<list_len; i++)
    {
        if(curr->ctrlr!=sender)
        {
            //broadcast
            if(fork()==0)
            {
                recepient=curr->ctrlr;
                _broadcast_run(recepient, cmds);
                goto exit;
            }
        }
        curr=curr->nxt;
    }

    if(_unlock(mcall2))
    {
        fprintf(stderr, "\n[-]Unlock failed for %s", inet_ntoa(sender->addr.sin_addr));
        ret=1;
        goto exit;
    }

exit:
    if(getpid()==main_pid)
    {
        deallocate(mcall2, "struct mutex_call", 1);
        deallocate(fcall, "struct func_call", 1);
        return ret;   
    }
}

void _broadcast_run(struct controller *recepient, char *cmds)
{
    if(snd(recepient->bcast_sock, cmds, "send broadcast msg", 0))
    {
        fprintf(stderr, "\n[-]Error in sending bcast %s to %s\n", cmds, inet_ntoa(recepient->addr.sin_addr));
    }
}

void *cleanup_run(void *a)
{

}

int send_pkt_back(struct controller *recepient, char *cmds, struct mutex_call *mcall)
{
}
