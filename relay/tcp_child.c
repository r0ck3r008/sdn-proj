#define NEEDS_STRUCT
#define _GNU_SOURCE

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<signal.h>
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
    int hard_exit=0, local_sock;
    struct sockaddr_in addr;
    struct controller *cli=(struct controller *)allocate("struct controller", 1);
    socklen_t len=sizeof(struct sockaddr_in);
    pid_t parent_pid=getpid(), child_pid;
    char *local_cmds;

    if((local_sock=sock_create_local(".sock", 0))==-1)
    {
        _exit(-1);        
    }

    for(int i=0, tag, cli_sock, stat;;)
    {
        local_cmds=(char *)allocate("char", 512);

        if(i==1)
        {
            pthread_t cleanup_tid;
            if((stat=pthread_create(&cleanup_tid, NULL, cleanup_run, NULL))!=0)
            {
                fprintf(stderr, "\n[-]Error in running cleanup thread: %s\n", strerror(stat));
                //hard exit as this is a critical element
                hard_exit=1;
                break;
            }
        }

        if((cli_sock=accept(tcp_sock, (struct sockaddr *)&addr, &len))==-1)
        {
            fprintf(stderr, "\n[-]Error in accepting client num %d: %s\n", i, strerror(errno));
            continue;
        }

        tag=i;
        sprintf(local_cmds, "add:%d:%s:%d:%d\n", tag, inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), cli_sock);
        cli->bcast_sock=cli_sock;
        memcpy(&cli->addr, &addr, sizeof(struct sockaddr_in));

        child_pid=fork();
        if(child_pid==0)
        {
            //child
            _cli_run(cli, tag);
            break;
        }
        else
        {
            //parent
            if(snd(cli_sock, local_cmds, "send to UDS the client info", 0))
            {
                fprintf(stderr, "\n[-]Error in writing for client %s:%s\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
                kill(child_pid, SIGTERM);
            }
            explicit_bzero(cli, sizeof(struct controller));
            i++;
        }
    }

    if(getpid()==parent_pid)
    {
        if(hard_exit)
        {
            kill(child_pid, SIGTERM);
        }
        deallocate(cli, "struct controller", 1);
    }
}

void _cli_run(struct controller *cli, int tag)
{
    int local_sock;
    char *local_cmds=(char *)allocate("char", 512), *local_cmdr;
    char *cmds=(char *)allocate("char", 512), *cmdr=(char *)allocate("char", 512);
    sprintf(cmdr, "genosis");

    //make UDS connecction
    if((local_sock=sock_create_local(".sock", 0))==-1)
    {
        goto exit;
    }
    
    //connect back
    if(_connect_back(cli))
    {
        goto exit;
    }
    
    for(cmdr; strcmp(cmdr, "END");)
    {
        deallocate(cmdr, "char", 512);

        if((cmdr=rcv(cli->sock, "receive from client", 1))==NULL)
        {
            fprintf(stderr, "\n[-]Error in receiving for client %s\n", inet_ntoa(cli->addr.sin_addr));
            break;
        }

        if(!strcmp(strtok(cmdr, ":"), "BROADCAST"))
        {
            //broadcast
        }
        else if(!strcmp(strtok(cmdr, ":"), "FOUND"))
        {
            //snd_pkt back
        }
    }

exit:
    deallocate(local_cmdr, "char", 512);
    deallocate(cmdr, "char", 512);
    deallocate(cli, "struct controller", 1);
}

int _connect_back(struct controller *sender)
{
    if((sender->sock=sock_create(inet_ntoa(sender->addr.sin_addr), 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in connecting back\n");
        return -1;
    }
    return 0;
}
