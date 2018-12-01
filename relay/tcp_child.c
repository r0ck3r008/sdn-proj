#define NEEDS_STRUCT
#define _GNU_SOURCE

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
#include"snd_rcv.h"
#include"list.h"
#include"allocate.h"
#include"sock_create.h"
#include"broadcast.h"

void tcp_child()
{
    int stat, i, exp_cli=10;
    ctrlr_start==NULL;
    socklen_t len=sizeof(struct sockaddr_in);
    pthread_t tid[exp_cli];
    char *retval;

    for(i=0; i<exp_cli; )
    {
        if((cli[i].bcast_sock=accept(tcp_sock, (struct sockaddr *)&cli[i].addr, &len))==-1)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d:%s\n", i, strerror(errno));
            continue;
        }

        if((stat=pthread_create(&tid[i], NULL, cli_run, &cli[i]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread for %s:%d: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), strerror(stat));
            continue;
        }

        i++;
        continue;
    }

    //initiate cleanup thread
    pthread_t cleanup_tid;
    if((stat=pthread_create(&cleanup_tid, NULL, cleanup_run, NULL))!=0)
    {
        fprintf(stderr, "\n[-]Error in running cleanup thread: %s\n", strerror(stat));
        //hard exit means critical element
        _exit(-1);
    }

    cli_num=i;
    for(i=0; i<cli_num; )
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client at %s:%d: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), strerror(stat));
            continue;
        }
        
        printf("\n[!]Client %s:%d joined with exit msg: %s\n", inet_ntoa(cli[i].addr.sin_addr), ntohs(cli[i].addr.sin_port), (char *)retval);
        free(retval);
        i++;
    }
}

void *cli_run(void *a)
{
    struct controller *client=(struct controller *)a;
    char *cmdr=(char *)allocate("char", 512), *retval=(char *)allocate("char", 128);
    sprintf(cmdr, "genisis");
    sprintf(retval, "genisis");


    sleep(1);
    if(connect_back(client))
    {
        sprintf(retval, "\n[-]Error in connecting back\n");
        goto exit;
    }

    for(;;)
    {
        free(cmdr);
        
        if((cmdr=rcv(client, client->sock, "receive from client", retval))==NULL)
        {
            fprintf(stderr, "%s", retval);
            continue;
        }

        if(strcasestr(strtok(cmdr, ":"), "broadcast")!=NULL)
        {
            //call broadcast
            if(broadcast(client, strtok(NULL, ":")))
            {
                sprintf(retval, "\n[-]Error in broadcasting for %s:%d:%s\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port), retval);
                fprintf(stderr, "\n[-]%s\n", retval);
                continue;
            }
        }
        else if(!strcmp(cmdr, "END"))
        {
            sprintf(retval, "\n[!]Exiting client %s:%d\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port));
            break;

        }
        else
        {
            //call send reply
            if(send_pkt_back(client, (int)strtol(strtok(cmdr, ":"), NULL, 10), strtok(NULL, ":")))
            {
                sprintf(retval, "\n[-]Error in sending pkt back for %s:%d: %s\n", inet_ntoa(client->addr.sin_addr), ntohs(client->addr.sin_port), retval);
                fprintf(stderr, "\n[-]%s\n", retval);
                continue;
            }
        }
    }

    exit:
    pthread_exit(retval);

}

int connect_back(struct controller *sender)
{
    if((sender->sock=sock_create(inet_ntoa(sender->addr.sin_addr), 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in connecting back\n");
        return -1;
    }
}
