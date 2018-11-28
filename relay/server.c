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


int server_workings(char *argv)
{
    //make pipe
    pipe_name="/tmp/.pipe";
    if(mkfifo(pipe_name, 0644)==-1)
    {
        fprintf(stderr, "\n[-]Error in making pipe: %s\n", strerror(errno));
        return 1;
    }

    pthread_t alarm_th;
    int stat, a=1;
    struct alarm_st a_s;
    a_s.time=10;
    a_s.a=&a;
    pid_t child_pid=fork();

    if(child_pid!=0)
    {
        //parent
        pthread_t pipe_rcv_tid;
        //start pipe_rcv process
        if((stat=pthread_create(&pipe_rcv_tid, NULL, pipe_rcv, NULL))!=0)
        {
            fprintf(stderr, "\n[-]Error in creating the pipe_rcv thread: %s\n", strerror(stat));
            sleep(5);
            return 1;
        }
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
        if((tcp_sock=sock_create(argv, 1))==-1)
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
        if((udp_sock=sock_create(argv, 3))==-1)
        {
            return 1;
        }
        //child-> udp_server
        udp_child();
    }
}
