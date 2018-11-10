#ifndef ALARM_H
#define ALARM_H


struct alarm_st
{
    int time;
    int *a;
};
void *alarm_run(void *);
int set_alarm(struct alarm_st *);

#endif
