#define NEEDS_STRUCT

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<signal.h>
#include<pthread.h>
#include<errno.h>

#include"global_defs.h"
#include"udp_child.h"
#include"tcp_child.h"
#include"alarm.h"
#include"sock_create.h"
#include"pipe_rcv.h"
#include"server.h"


int server_workings(char *argv1, char *argv2)
{
    //make pipe
    if(make_if_not_exists(argv2)==-1)
    {
        return 1;
    }

    pthread_t alarm_th;
    int stat, a=1;
    struct alarm_st a_s;
    a_s.time=10;
    a_s.a=&a;

    pthread_t pipe_rcv_tid;
    //start pipe_rcv process
    if((stat=pthread_create(&pipe_rcv_tid, NULL, pipe_rcv, argv2))!=0)
    {
        fprintf(stderr, "\n[-]Error in creating the pipe_rcv thread: %s\n", strerror(stat));
        sleep(5);
        return 1;
    }

    pid_t child_pid=fork();

    if(child_pid!=0)
    {
        //parent
        //handle udp sub-process in saperate thread
        for(int i=0; a;)
        {
            if(!i)
            {
               if(set_alarm(&a_s))
               {
                   fprintf(stderr, "\n[-]Error in setting the alarm\n");
                   break;
               }
               i++;
            }
        }
        if((stat=kill(child_pid, SIGTERM))!=0)
        {
            fprintf(stderr, "\n[-]Error in killing udp server: %s\n", strerror(stat));
            _exit(-1);
        }
        close(udp_sock);
            
        //handle tcp server now
        if((tcp_sock=sock_create(argv1, 1))==-1)
        {
            return 1;
        }
        if(tcp_child())
        {
            return 1;
        }
    }
    else
    {
        //create socket
        if((udp_sock=sock_create(argv1, 3))==-1)
        {
            return 1;
        }
        //child-> udp_server
        udp_child(argv2);
    }
}

int make_if_not_exists(char *name)
{
    struct stat stat_st;
    explicit_bzero(&stat_st, sizeof(struct stat));

    if(stat(name, &stat_st)==0)
    {
        if(remove(name)==-1)
        {
            fprintf(stderr, "\n[-]%s exists but remove failed, exiting: %s\n", name, strerror(errno));
            return 1;
        }
        printf("\n[!]Successfully removed pre-existing %s, now creating new.\n");
    }

    if(mkfifo(name, 0644)==-1)
    {
        fprintf(stderr, "\n[-]Error in making fifo %s: %s\n", name, strerror(errno));
        return 1;
    }

    printf("\n[!]Successfully crated %s\n", name);
    return 0;
}
