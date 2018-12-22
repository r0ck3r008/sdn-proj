#define NEEDS_STRUCT

#include<stdio.h>
#include<string.h>
#include<stdarg.h>
#include<pthread.h>
#include<errno.h>

#include"global_defs.h"
#include"lock_and_exec.h"
#include"broadcast.h"
#include"allocate.h"
#include"list.h"

int lock_and_exec(pthread_mutex_t *lock, struct func_call *fcall, int num, ...)
{
    va_list lst;
    explicit_bzero(&lst, sizeof(va_list));
    va_start(lst, num);
    int stat, ret=0;

    if((stat=pthread_mutex_lock(lock))!=0)
    {
        fprintf(stderr, "\n[-]Error in locking mutex: %s\n", strerror(stat));
        ret=1;
        goto exit;
    }

    //do function here
    if(fcall->add!=NULL)
    {
        union node *new=va_arg(lst, union node *), *start=va_arg(lst, union node *);
        int flag=va_arg(lst, int);
        add_node(new, start, flag);
    }
    else if(fcall->find!=NULL)
    {
        union node *ret=va_arg(lst, union node *), *start=va_arg(lst, union node *);
        int tag=va_arg(lst, int);
        ret=fcall->find(start, tag);
    }

end:
    if((stat=pthread_mutex_unlock(lock))!=0)
    {
        fprintf(stderr, "\n[-]Error in unlocking mutex: %s\n", strerror(stat));
        ret=1;
    }

exit:
    va_end(lst);
    deallocate(fcall, "struct func_call", 1);
    return ret;
}

struct func_call *alloc_fcall(int flag)
{
    struct func_call *fcall=(struct func_call *)allocate("struct func_call", 1);
    fcall->add=NULL;
    fcall->find=NULL;
    fcall->del=NULL;
    fcall->bcast=NULL;
    fcall->snd=NULL;

    switch(flag)
    {
     case 0:
         fcall->add=add_node;
         break;
     case 1:
         fcall->find=find_node;
         break;
     case 2:
         fcall->del=del_node;
         break;
     case 3:
         fcall->bcast=broadcast;
         break;
     case 4:
         fcall->snd=send_pkt_back;
    }
}
