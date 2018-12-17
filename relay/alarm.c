#include<stdio.h>
#include<pthread.h>
#include<unistd.h>
#include<string.h>

#include"alarm.h"

int set_alarm(struct alarm_st *a_s)
{
    int stat, ret=0;
    pthread_t alarm_tid;

    if((stat=pthread_create(&alarm_tid, NULL, _alarm_run, a_s))!=0)
    {
        fprintf(stderr, "\n[-]Error in setting alarm for %s: %s\n", strerror(stat));
        ret=1;
    }

    return ret;
}

void *_alarm_run(void *alarm)
{
    struct alarm_st *a_s=(struct alarm_st *)alarm;

    sleep(a_s->time);
    *a_s->a=0;

    printf("[!]Time's up\n");

    pthread_exit(NULL);
}
