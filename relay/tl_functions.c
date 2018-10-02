#define NEEDS_STRUCT
#define NEEDS_NORMIE

#include"global_defs.h"
#include"tl_functions.h"
#include"sock_create.h"
#include"snd_rcv.h"
#include"link_normie.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

struct alarm_st
{
    int time;
    int *a;
};

int udp_server(char *argv)
{
    normie_start=NULL;
    struct alarm_st a_s;
    int a=1, ret=0, stat;
    pthread_t alarm_tid;
    char *cmdr=(char *)allocate("char", 512), *addr=(char *)allocate("char", 50);
    sprintf(cmdr, "genisis");
    sprintf(addr, "genisis");

    if((udp_sock=sock_create(argv, 3))==-1)
    {
        return 1;
    }
    
    for(int i=0; a;)
    {
        free(cmdr);
        free(addr);
        if(!i)
        {
            //alarm
            a_s.time=5;
            a_s.a=&a;
            if((stat=pthread_create(&alarm_tid, NULL, alarm_run, &a_s))!=0)
            {
                fprintf(stderr, "\n[-]Error in setting alarm: %s\n", strerror(stat));
                ret=1;
                break;
            }
        }
        char *addr=(char *)allocate("char", 50);
        if((cmdr=rcv_frm(addr))==NULL)
        {
            fprintf(stderr, "\n[-]Error in receving: %d\n", i);
            ret=1;
            break;
        }
        add_node(addr);

        i++;
    }

    return ret;
}

void *alarm_run(void *alarm)
{
    struct alarm_st *a_s=(struct alarm_st *)alarm;

    sleep(a_s->time);
    *a_s->a=0;
    pthread_exit(NULL);
}

int tcp_server(char *argv)
{
    if((tcp_sock=sock_create(argv, 1))==-1)
    {
        return 1;
    }
}
