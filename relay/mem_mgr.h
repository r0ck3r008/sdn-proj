#ifndef MEM_MGR_H
#define MEM_MGR_H

struct func_call
{
    void (*add)(union node *, union node *, int);
    union node * (*find)(union node *, int);
    int (*bcast)(struct controller *, char *);
    int (*snd_bk)(void);
}

int mem_mgr();
void *_local_cli_run(void *);
struct func_call *_init_fcall_st(int);
int _gain_lock_and_exec(pthread_mutex_t *, struct func_call *, int, ...);

#endif
