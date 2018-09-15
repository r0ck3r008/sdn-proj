#define NEEDS_CLIENT_STRUCT

#include"global_defs.h"
#include"server.h"
#include"dbconnect.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

int server_init(char *argv1)
{
    if((server_sock=socket(AF_INET, SOCK_DGRAM, 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in creating socket server: %s\n", strerror(errno));
        return 1;
    }
    printf("\n[!]Socket for server created successfully\n");

    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(strtok(argv1, ":"));
    addr.sin_port=htons((int)strtol(strtok(NULL, ":"), NULL, 10));

    if(bind(server_sock, (struct sockaddr *)&addr, sizeof(addr))==-1)
    {
        fprintf(stderr, "\n[-]Error in binding the socket: %s\n", strerror(errno));
        return 1;
    }
    printf("\n[!]Server socket bound successfully\n");

    return 0;
}

void server_workings()
{
    pthread_t tid[10];
    struct client cli[10];
    socklen_t len=sizeof(struct sockaddr_in);
    char *msg, *retval;
    int stat;

    for(int i=0; i<10; i++)
    {
        msg=(char *)allocate("char", 128);

        if(recvfrom(server_sock, msg, sizeof(char)*128, 0, (struct sockaddr *)&cli[i].cli, &len)==-1)
        {
            fprintf(stderr, "\n[-]Error in recvfrom: %s\n", strerror(errno));
            continue;
        }

        cli[i].msg=msg;

        if((stat=pthread_create(&tid[i], NULL, cli_run, &cli[i]))!=0)
        {
            fprintf(stderr, "\n[-]Error in running client: %s:%d: %s\n", inet_ntoa(cli[i].cli.sin_addr), ntohs(cli[i].cli.sin_port), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<10; i++)
    {
        if((stat=pthread_join(tid[i], (void **)&retval))!=0)
        {
            fprintf(stderr, "\n[-]Error in joining to thread %d: %s\n", i, (char *)retval);
            continue;
        }
        printf("\n[!]Successfully joined to thread %d:%s\n", i, (char *)retval);
        free(retval);
    }
}

void *cli_run(void *a)
{
    struct client *cli=(struct client *)a;
    socklen_t len=sizeof(struct sockaddr_in);
    int privilage;
    char *retval=(char *)allocate("char", 64*sizeof(char)), *cmds=(char *)allocate("char", 512*sizeof(char));

    if(!strcmp(cli->msg, "CONTROLLER UP") || !strcmp(cli->msg, "CONTROLLER DOWN"))
    {
        privilage=1;
    }
    else
    {
        privilage=0;
    }

    if(query_db(cli, cmds, privilage))
    {
        sprintf(retval, "Query fail");
    }
    
    if(sendto(server_sock, cmds, sizeof(char)*512, 0, (struct sockaddr *)&cli->cli, len)==-1)
    {
        fprintf(stderr, "\n[-]Error in sending ack back to %s: %s\n", inet_ntoa(cli->cli.sin_addr), strerror(errno));
        sprintf(retval, "Error in sending ACK");
    }

    pthread_exit(retval);
}
