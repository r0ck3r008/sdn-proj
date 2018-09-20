#define _GNU_SOURCE
#define NEEDS_STRUCT

#include"global_defs.h"
#include"server.h"
#include"broadcast.h"
#include"allocate.h"
#include"snd_rcv.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

struct alarm_struct
{
    int time;
    int *a;
};

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
    int stat, a=1;
    char *retval;
    pthread_t tid[10];
    cli=(struct controller *)allocate("struct controller", 10);
    socklen_t len=sizeof(struct controller);

    for(cli_num=0; a; cli_num++)
    {
        if(cli_num==0)
        {
            pthread_t alarm_thr;
            struct alarm_struct a_s;
            a_s.time=5;
            a_s.a=&a;

            if((stat=pthread_create(&alarm_thr, NULL, alarm_run, &a_s))!=0)
            {
                fprintf(stderr, "\n[-]Error in setting alarm: %s\n", strerror(stat));
                _exit(-1);
            }
        }

        if((cli[cli_num].sock=accept(server_sock, (struct sockaddr *)&cli[cli_num].addr, &len))==-1)
        {
            fprintf(stderr, "\n[-]Error in accepting client %d:%s\n", cli_num, strerror(errno));
            continue;
        }

        if((stat=pthread_create(&tid[cli_num], NULL, cli_run, &cli[cli_num]))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating thread for %s: %s\n", inet_ntoa(cli[cli_num].addr.sin_addr), strerror(stat));
            continue;
        }
    }

    for(int i=0; i<cli_num; i++)
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
    
    char *cmds, *cmdr=(char *)allocate("char", 512), *retval=(char *)allocate("char", 256);
    sprintf(cmdr, "start");
    for( ;strcasestr(cmdr, "END")==NULL; )
    {
        free(cmdr);
        //receive command
        if((cmdr=rcv(cli, "receive command from client", retval))==NULL)
        {
            fprintf(stderr, retval);
            break;
        }
        
        //broadcast or receive output
        if(strcasestr("BROADCAST", cmdr)!=NULL)
        {
            //broadcast query
            if(broadcast(cli, cmdr))
            {
                sprintf(retval, "Error in broadcast");
                fprintf(stderr, retval);
                _exit(-1);
                break;
            }
        }
        else
        {
            //parse packet to find broadcast reply id

        }
    }

    pthread_exit(retval);
}

void *alarm_run(void *a_struct)
{
    struct alarm_struct a_s= *(struct alarm_struct *)a_struct;

    sleep(a_s.time);
    *a_s.a=0;

    pthread_exit(NULL);
}
