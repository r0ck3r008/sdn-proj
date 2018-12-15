#define NEEDS_ALL
#define _GNU_SOURCE

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
#include"tcp_child.h"
#include"snd_rcv.h"
#include"list.h"
#include"allocate.h"
#include"sock_create.h"
#include"broadcast.h"

void tcp_child()
{
    int stat, i, tag;
    ctrlr_start==NULL;
    bmn_start=NULL;
    socklen_t len=sizeof(struct sockaddr_in);
    union node *new=(union node *)allocate("union node", 1);
    new->ctrlr=(struct controller *)allocate("struct controller", 1);
    pid_t parent_pid=getpid();

    for(i=0;;)
    {
        if((new->ctrlr->bcast_sock=accept(tcp_sock, (struct sockaddr *)&new->ctrlr->addr, &len))==-1)
        {
            fprintf(stderr, "\n[-]Error in accepting client num %d: %s\n", i, strerror(errno));
            continue;
        }
        tag=i;
        new->tag=tag;
        new->ctrlr->id=tag;
        add_node(new, ctrlr_start, 0);
        if(fork()==0)
        {
            //child
            union node *curr=find_node(ctrlr_start, tag);
            cli_run(curr);
            break;
        }
        else
        {
            //parent
            explicit_bzero(new->ctrlr, sizeof(struct controller));
            explicit_bzero(new, sizeof(union node));
            i++;
        }
    }

    //initiate cleanup thread only in parent
    if(getpid()==parent_pid)
    {
        cli_num=i;
        pthread_t cleanup_tid;
        if((stat=pthread_create(&cleanup_tid, NULL, cleanup_run, NULL))!=0)
        {
            fprintf(stderr, "\n[-]Error in running cleanup thread: %s\n", strerror(stat));
            //hard exit means critical element
            _exit(-1);
        }
    
        deallocate(new, "union node", 1);
    }
}

void cli_run(union node *client)
{
    char *cmdr=(char *)allocate("char", 512), *retval=(char *)allocate("char", 128);
    sprintf(cmdr, "genisis");
    sprintf(retval, "genisis");

    sleep(1);
    if(connect_back(client->ctrlr))
    {
        fprintf(stderr, "\n[-]Error in connecting back\n");
        goto exit;
    }

    for(;;)
    {
        explicit_bzero(retval, sizeof(char)*128);
        deallocate(cmdr, "char", 512);
        
        if((cmdr=rcv(client->ctrlr, client->ctrlr->sock, "receive from client", retval))==NULL)
        {
            fprintf(stderr, "%s", retval);
            continue;
        }

        if(strcasestr(strtok(cmdr, ":"), "broadcast")!=NULL)
        {
            //call broadcast
            if(broadcast(client->ctrlr, cmdr))
            {
                fprintf(stderr, "\n[-]Error in broadcasting for %s:%d\n", inet_ntoa(client->ctrlr->addr.sin_addr), ntohs(client->ctrlr->addr.sin_port));
                continue;
            }
        }
        else if(!strcmp(cmdr, "END"))
        {
            sprintf(retval, "\n[!]Exiting client %s:%d\n", inet_ntoa(client->ctrlr->addr.sin_addr), ntohs(client->ctrlr->addr.sin_port));
            break;

        }
        else
        {
            //call send reply
            if(send_pkt_back(client->ctrlr, (int)strtol(strtok(cmdr, ":"), NULL, 10), strtok(NULL, ":")))
            {
                sprintf(retval, "\n[-]Error in sending pkt back for %s:%d: %s\n", inet_ntoa(client->ctrlr->addr.sin_addr), ntohs(client->ctrlr->addr.sin_port), retval);
                fprintf(stderr, "\n[-]%s\n", retval);
                continue;
            }
        }
    }

exit:
    deallocate(retval, "char", 128);
    deallocate(cmdr, "char", 512);
}

int connect_back(struct controller *sender)
{
    if((sender->sock=sock_create(inet_ntoa(sender->addr.sin_addr), 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in connecting back\n");
        return -1;
    }
}
