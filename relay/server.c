#define NEEDS_STRUCT

#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<signal.h>
#include<pthread.h>

#include"global_defs.h"
#include"udp_child.h"
#include"tcp_child.h"
#include"alarm.h"
#include"sock_create.h"
#include"server.h"


int server_workings(char *argv)
{
    pthread_t alarm_th;
    int stat, a=1;
    struct alarm_st a_s;
    a_s.time=5;
    a_s.a=&a;
    pid_t child_pid=fork();

    if(child_pid!=0)
    {
        //parent
        //handle udp sub-process
        for(int i=0; a; i++)
        {
            if(!i)
            {
               if(set_alarm(&a_s))
               {
                   fprintf(stderr, "\n[-]Error in setting the alarm\n");
                   break;
               } 
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
