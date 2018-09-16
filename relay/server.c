#define _GNU_SOURCE
#define NEEDS_CONTROLLER_STRUCT

#include"global_defs.h"
#include"server.h"
#include"allocate.h"
#include"snd_rcv.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

int create_sock(char *argv1)
{
    if((server_sock=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in creating socket: %s\n", strerror(errno));
        return 1;
    }

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(strtok(argv1, ":"));
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));

    if(bind(server_sock, (struct sockaddr *)&addr, sizeof(addr))==-1)
    {
        fprintf(stderr, "\n[-]Error in binding the server socket: %s\n", strerror(errno));
        return 1;
    }
    printf("\n[!]Server socket created and bound successfully\n");

    if(listen(server_sock, 5)==-1)
    {
        fprintf(stderr, "\n[-]Error in listning on server socket: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}

int server_workings()
{
    int stat;
    char *retval;
    pthread_t tid[10];
    cli=(struct controller *)allocate("struct controller", 10);
    socklen_t len=sizeof(struct controller);

    for(int i=0; i<10; i++)
    {
        if((cli[i].sock=accept(server_sock, (struct sockaddr *)&cli[i].addr, &len))==-1)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d:%s\n", i, strerror(errno));
            continue;
        }

        if((stat=pthread_create(&tid[i], NULL, cli_run, &cli[i]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread for %s: %s\n", inet_ntoa(cli[i].addr.sin_addr), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<10; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to client at: %s: %s\n", inet_ntoa(cli[i].addr.sin_addr), strerror(stat));
            continue;
        }

        printf("\n[!]Joined to controller at: %s\n", inet_ntoa(cli[i].addr.sin_addr));
        free(retval);
    }
}

void *cli_run(void *a)
{
    struct controller *cli=(struct controller *)a;
    printf("\n[!]Controller %s:%d connected\n", inet_ntoa(cli->addr.sin_addr), ntohs(cli->addr.sin_port));
    
    char *cmds, *cmdr, *retval=(char *)allocate("char", 256);
    
    for( ;!strcmp(cmdr, "END"); )
    {
        //receive command
        if((cmdr=rcv(cli, "receive command from client", retval))==NULL)
        {
            fprintf(stderr, retval);
            explicit_bzero(retval, sizeof(char)*256);
            continue;
        }
        
        //broadcast or receive output
        if(strcasestr("BROADCAST", cmdr)!=NULL)
        {
            //broadcast query
            if(broadcast())
        }
        else
        {
            //parse packet to find broadcast reply id

        }
    }
}
