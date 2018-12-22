#ifndef LOCK_AND_EXEC_H
#define LOCK_AND_EXEC_H

struct func_call
{
    void (*add)(union node *, union node *, int);
    union node *(*find)(union node *, int);
    int (*del)(union node *, int, int);
    int (*bcast)(struct controller *, char *);
    int (*snd)(struct controller *, int, char *);
};


int lock_and_exec(pthread_mutex_t *, struct func_call *, int, ...);
struct func_call *alloc_fcall(int);

#endif
