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
#include"tcp_child.h"
#include"list.h"
#include"allocate.h"
#include"snd_rcv.h"

int broadcast(struct controller *sender, char *cmds, struct mutex_call *mcall)
{
    int list_len=0, ret=0;
    pid_t main_pid=getpid();
    struct mutex_call *mcall2=alloc_mcall(0, 2, mcall->ctrlr_ro, mcall->ctrlr);

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
        return ret;
    }
}

int _broadcast_helper(struct mutex_call *mcall, struct controller *sender, char *cmds, int *len)
{
    int ret=0;
    struct mutex_call *mcall2=alloc_mcall(0, 2, mcall->ctrlr_ro, mcall->ctrlr);
    struct func_call *fcall=alloc_fcall(1);
    union node *new=_alloc_new(0);
    new->bmn->sender=sender;
    new->bmn->msg=cmds;
    new->bmn->done=0;

    if(lock_and_exec(mcall2, fcall, 2, ctrlr_start, len))
    {
        fprintf(stderr, "\n[-]Error in finding list len for %s\n", inet_ntoa(sender->addr.sin_addr));
        ret=1;
        goto exit;
    }

    deallocate(fcall, "struct func_call", 1);
    deallocate(mcall2, "struct mutex_call", 1);
    mcall2=alloc_mcall(2, 1, mcall->bmn);
    fcall=alloc_fcall(0);

    if(lock_and_exec(mcall, fcall, 3, new, bmn_start, 1))
    {
        fprintf(stderr, "\n[-]Error in adding bmn for %s\n", inet_ntoa(sender->addr.sin_addr));
        ret=1;
        goto exit;
    }

exit:
    deallocate(mcall2, "struct mutex_call", 1);
    deallocate(fcall, "struct func_call", 1);
    deallocate(new->bmn, "struct broadcast_msg_node", 1);
    deallocate(new, "union node", 1);
    return ret;
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
