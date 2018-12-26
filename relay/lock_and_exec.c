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

int lock_and_exec(struct mutex_call *mcall, struct func_call *fcall, int num, ...)
{
    va_list lst;
    explicit_bzero(&lst, sizeof(va_list));
    va_start(lst, num);
    int stat, ret=0;

    if(_lock(mcall))
    {
        fprintf(stderr, "\n[-]Error in locking\n");
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

    if(_unlock(mcall))
    {
        fprintf(stderr, "\n[-]Error in unlocking\n");
        ret=1;
    }

exit:
    va_end(lst);
    deallocate(fcall, "struct func_call", 1);
    deallocate(mcall, "struct mutex_call", 1);
    return ret;
}

int _lock(struct mutex_call *mcall)
{
    int ret=0, stat;
    //locking mechanism
    if(mcall->ctrlr!=NULL)
    {
        int ctrlr_needs_locking=1, ctrlr_ro_locked=0;
        if(mcall->ctrlr_ro!=NULL)
        {
            ctrlr_ro_locked=1;
            //lock ctrlr_ro
            if((stat=pthread_mutex_lock(mcall->ctrlr_ro))!=0)
            {
                fprintf(stderr, "\n[-]Error in locking ctrlr_ro: %s\n", strerror(errno));
                ret=1;
                goto exit;
            }
            if(++ctrlr_rc!=1)
            {
                ctrlr_needs_locking=0;
            }
            
        }
        if(ctrlr_needs_locking)
        {
            if((stat=pthread_mutex_lock(mcall->ctrlr))!=0)
            {
                fprintf(stderr, "\n[-]Error in locking ctrlr: %s\n", strerror(errno));
                ret=1;
                goto exit;
            }
        }

        if(ctrlr_ro_locked)
        {
            if((stat=pthread_mutex_unlock(mcall->ctrlr_ro))!=0)
            {
                fprintf(stderr, "\n[-]Error in unlocking ctrlr_ro: %s\n", strerror(errno));
                ret=1;
                goto exit;
            }
        }

    }
    else if(mcall->bmn!=NULL)
    {
        if((stat=pthread_mutex_lock(mcall->bmn))!=0)
        {
            fprintf(stderr, "\n[-]Error in locking bmn: %s\n", strerror(errno));
            ret=1;
            goto exit;
        }
    }

exit:
    return ret;
}

int _unlock(struct mutex_call *mcall)
{
    int stat, ret=0;

    if(mcall->ctrlr!=NULL)
    {
        int ctrlr_needs_unlocking=1, ctrlr_ro_locked=0;
        if(mcall->ctrlr_ro!=NULL)
        {
            ctrlr_ro_locked=1;
            if((stat=pthread_mutex_lock(mcall->ctrlr_ro))!=0)
            {
                fprintf(stderr, "\n[-]Error in locking ctrlr_ro: %s\n", strerror(errno));
                ret=1;
                goto exit;
            }
            if(--ctrlr_rc!=0)
            {
                ctrlr_needs_unlocking=0;
            }
        }

        if(ctrlr_needs_unlocking)
        {
            if((stat=pthread_mutex_unlock(mcall->ctrlr))!=0)
            {
                fprintf(stderr, "\n[-]Error in unlocking ctrlr: %s\n", strerror(errno));
                ret=1;
                goto exit;
            }
        }

        if(ctrlr_ro_locked)
        {
            if((stat=pthread_mutex_unlock(mcall->ctrlr_ro))!=0)
            {
                fprintf(stderr, "\n[-]Error in unlocking ctrlr_ro: %s\n", strerror(errno));
                ret=1;
                goto exit;
            }
        }
    }
    else if(mcall->bmn!=NULL)
    {
        if((stat=pthread_mutex_unlock(mcall->bmn))!=0)
        {
            fprintf(stderr, "\n[-]Error in unlocking bmn: %s\n", strerror(errno));
            ret=1;
            goto exit;
        }
    }

exit:
    return ret;
}

struct func_call *alloc_fcall(int flag)
{
    struct func_call *fcall=(struct func_call *)allocate("struct func_call", 1);
    fcall->add=NULL;
    fcall->find=NULL;
    fcall->del=NULL;

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
    }
}

struct mutex_call *alloc_mcall(int flag, int num, ...)
{
    va_list lst;
    va_start(lst, num);

    struct mutex_call *mcall=(struct mutex_call *)allocate("struct mutex_call", 1);
    mcall->ctrlr=NULL;
    mcall->ctrlr_ro=NULL;
    mcall->bmn=NULL;

    switch(flag)
    {
     case 0: //read access to ctrlr
         mcall->ctrlr_ro=va_arg(lst, pthread_mutex_t *);

     case 1: //write access to ctrlr
         mcall->ctrlr=va_arg(lst, pthread_mutex_t *);
         break;
     case 2: //write access to bmn
         mcall->bmn=va_arg(lst, pthread_mutex_t *);
    }

    va_end(lst);
    return mcall;
}
