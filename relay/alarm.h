#ifndef ALARM_H
#define ALARM_H


struct alarm_st
{
    int time;
    int *a;
};
int set_alarm(struct alarm_st *);
void *_alarm_run(void *);

#endif
