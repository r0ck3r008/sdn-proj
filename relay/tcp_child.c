#define _GNU_SOURCE
#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<pthread.h>
#include<errno.h>

#include"global_defs.h"
#include"tcp_child.h"
#include"broadcast.h"
#include"allocate.h"
#include"lock_and_exec.h"
#include"snd_rcv.h"
#include"sock_create.h"

pthread_mutex_t ctrlr_mtx=PTHREAD_MUTEX_INITIALIZER;

int tcp_child()
{
    ctrlr_rc=0;
    int stat, hard_exit=0, ret=0;
    pthread_t tid[10];
    socklen_t len=sizeof(struct sockaddr_in);
    union node *new;
    struct func_call *fcall=alloc_fcall(0);

    //main loop
    for(int i=0;;)
    {
        new=_allocate_new(1);

        if(i==1)
        {
            pthread_t cleanup_id;
            if((stat=pthread_create(&cleanup_id, NULL, cleanup_run, NULL))!=0)
            {
                fprintf(stderr, "\n[-]Error in starting cleanup thread: %s\n", strerror(errno));
                hard_exit=i;
                break;
            }
        }

        if((new->ctrlr->bcast_sock=accept(tcp_sock, (struct sockaddr *)&new->ctrlr->addr, &len))==-1)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d: %s\n", i, strerror(errno));
            continue;
        }

        //block and do
        new->tag=i;
        if(lock_and_exec(&ctrlr_mtx, fcall, 3, new, ctrlr_start, 0))
        {
            fprintf(stderr, "\n[-]Error in adding node for %s:%d\n", inet_ntoa(new->ctrlr->addr.sin_addr), ntohs(new->ctrlr->addr.sin_port));
            hard_exit=i;
            break;
        }

        if((stat=pthread_create(&tid[i], NULL, _cli_run, &i))!=0)
        {
            fprintf(stderr, "\n[-]Error in starting thread for %s:%d: %s\n", inet_ntoa(new->ctrlr->addr.sin_addr), ntohs(new->ctrlr->addr.sin_port), strerror(stat));
            continue;
        }

        deallocate(new->ctrlr, "struct controller", 1);
        deallocate(new, "union node", 1);
        i++;
    }

    if(hard_exit)
    {
        for(int i=0; i<hard_exit; i++)
        {
            pthread_cancel(tid[i]);
        }
        ret=1;
    }
    deallocate(fcall, "struct func_call", 1);
    return ret;
}

void *_cli_run(void *a)
{
    int tag=*(int *)a;
    char *cmdr=(char *)allocate("char", 512);
    sprintf(cmdr, "genesis");
    struct func_call *fcall;
    union node *client;
    if((client=_client_helper(tag))==NULL)
    {
        goto exit;
    }

    for(cmdr; strcasestr(cmdr, "END")==NULL;)
    {
        deallocate(cmdr, "char", 512);

        if((cmdr=rcv(client->ctrlr->sock, "receive from client"))==NULL)
        {
            fprintf(stderr, "\n[-]Error in receving from %s:%d\n", inet_ntoa(client->ctrlr->addr.sin_addr), ntohs(client->ctrlr->addr.sin_port));
            break;
        }
        if(strcasestr(cmdr, "broadcast")!=NULL)
        {
            //broadcast
            if(broadcast(client->ctrlr, cmdr))
            {
                fprintf(stderr, "\n[-]Error in broadcasting for %s\n", inet_ntoa(client->ctrlr->addr.sin_addr));
                break;
            }
        }
        else if(strcasestr(cmdr, "found")!=NULL)
        {
            //snd_pkt back
            if(send_pkt_back(client->ctrlr, cmdr))
            {
                fprintf(stderr, "\n[-]Error in sending back to %s\n", inet_ntoa(client->ctrlr->addr.sin_addr));
                break;
            }
        }

    }
    
exit:

    printf("\n[!]Closing connection to %s:%d\n", inet_ntoa(client->ctrlr->addr.sin_addr), ntohs(client->ctrlr->addr.sin_port));
    //close sockets
    close(client->ctrlr->bcast_sock);
    close(client->ctrlr->sock);
    //deallocate buffer and structures
    fcall=alloc_fcall(2);
    if(lock_and_exec(&ctrlr_mtx, fcall, 3, client, 0, tag))
    {
        fprintf(stderr, "\n[-]Error in deleting node %s:%d\n", inet_ntoa(client->ctrlr->addr.sin_addr), ntohs(client->ctrlr->addr.sin_port));
    }
    deallocate(fcall, "struct func_call", 1);
    deallocate(cmdr, "char", 512);
    //exit
    pthread_exit(NULL);
}

union node *_client_helper(int tag)
{
    //block and find
    int hard_exit=0;
    union node *client;
    struct func_call *fcall=alloc_fcall(1);

    if(lock_and_exec(&ctrlr_mtx, fcall, 3, client, ctrlr_start, tag))
    {
        fprintf(stderr, "\n[-]Error in finding client node for tag %d\n", tag);
        goto exit;
    }

    //connect back
    if(_connect_back(client->ctrlr))
    {
        goto exit;
    }

exit:
    deallocate(fcall, "struct func_call", 1);
    if(hard_exit)
    {
        return NULL;
    }
    return client;
}

int _connect_back(struct controller *client)
{
    int ret=0;
    if((client->sock=sock_create(inet_ntoa(client->addr.sin_addr), 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in connecting back to client for %s:%d\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
        ret=client->sock;
    }
    return ret;
}

union node *_allocate_new(int flag)
{
    union node *new=(union node *)allocate("union node", 1);
    new->nxt=NULL;
    new->prev=NULL;

    if(flag)
    {
        new->ctrlr=(struct controller *)allocate("struct controller", 1);
        new->bmn=NULL;
    }
    else
    {
        new->bmn=(struct bcast_msg_node *)allocate("struct bcast_msg_node", 1);
        new->ctrlr=NULL;
    }

    return new;
}
