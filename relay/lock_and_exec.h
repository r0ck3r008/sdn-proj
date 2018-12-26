#ifndef LOCK_AND_EXEC_H
#define LOCK_AND_EXEC_H

struct func_call
{
    void (*add)(union node *, union node *, int);
    union node *(*find)(union node *, int);
    int (*del)(union node *, int, int);
};

struct mutex_call
{
    pthread_mutex_t *ctrlr, *ctrlr_ro, *bmn;
};


int lock_and_exec(struct mutex_call  *, struct func_call *, int, ...);
int _lock(struct mutex_call *);
int _unlock(struct mutex_call *);
struct func_call *alloc_fcall(int);
struct mutex_call *alloc_mcall(int, int, ...);

#endif
