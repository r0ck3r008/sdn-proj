#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<signal.h>
#include<pthread.h>

#include"global_defs.h"
#include"udp_child.h"
#include"tcp_child.h"
#include"sock_create.h"
#include"server.h"


struct alarm_st
{
    int time;
    int *a;
};

int server_workings(char *argv1, char *argv2)
{
    pthread_t alarm_th;
    int stat, a=1;
    struct alarm_st a_s;
    a_s.time=5;
    a_s.a=&a;
    pid_t child_pid;

    //create sockets
    if((udp_sock=sock_create(argv1, 3))==-1)
    {
        return 1;
    }
    if((tcp_sock=sock_create(argv2, 1))==-1)
    {
        return 1;
    }

    if((child_pid=fork())!=0)
    {
        //parent
        for(int i=0; a; i++)
        {
            if(!i)
            {
                if((stat=pthread_create(&alarm_th, NULL, alarm_run, &a_s))==0)
                {
                    fprintf(stderr, "\n[-]Error in setting alarm: %s\n", strerror(stat));
                    _exit(-1);
                }
            }
        }
        if((stat=kill(child_pid, SIGTERM))!=0)
        {
            fprintf(stderr, "\n[-]Error in killing udp server: %s\n", strerror(stat));
            _exit(-1);
        }
    }
    else
    {
        //child-> udp_server
        if(udp_child())
        {
            return 1;
        }
    }
    
    if(fork()==0)
    {
        if(tcp_child())
        {
            return 1;
        }
    }
}

void *alarm_run(void *alarm)
{
    struct alarm_st *a_s=(struct alarm_st *)alarm;

    sleep(a_s->time);

    pthread_exit(NULL);
}

