#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdarg.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>

#include"global_defs.h"
#include"mem_mgr.h"
#include"snd_rcv.h"
#include"sock_create.h"
#include"list.h"
#include"broadcast.h"
#include"allocate.h"

pthread_mutex_t ctrlr_start_mtx=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t bmn_start_mtx=PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t local_start_mtx=PTHREAD_MUTEX_INITIALIZER;

int mem_mgr()
{
    int sock;
    if((sock=sock_create_local(".sock", 1))==-1)
    {
        return 1;
    }
    
    //start of all linked lists
    ctrlr_start=NULL;
    bmn_start=NULL;
    local_start=NULL;

    //fcall to use add node
    struct func_call *fcall_st=_init_fcall_st(0);

    //for pthread creates
    int stat;
    pthread_t tid[10];

    //to have new node to add
    union node *new=(union node *)allocate("union node", 1);
    new->local =(struct local_struct *)allocate("struct local_struct", 1);
    socklen_t len=sizeof(new->local->addr);
  
    //main loop
    for(int i=0;;)
    {
        if((new->local->sock=accept(sock, (struct sockaddr *)&new->local->addr, &len))==-1)
        {
            fprintf(stderr, "\n[-]Error in accepting local connection %d: %s", i, strerror(errno));
            continue;
        }
        new->tag=i;

        if(_gain_lock_and_exec(&local_start_mtx, fcall_st, 3, local_start, new, 2))
        {
            continue;
        }

        if((stat=pthread_create(&tid[i], NULL, _local_cli_run, &i))==-1)
        {
            fprintf(stderr, "\n[-]Error in creating new thread for local client %d: %s\n", i, strerror(stat));
            continue;
        }

        explicit_bzero(new->local, sizeof(new->local));
        explicit_bzero(new, sizeof(union node));
        i++;
    }

    return 0;
}

void *_local_cli_run(void *a)
{
    //init func_call struct
    struct func_call *fcall_st=_init_fcall_st(1);

    //find local node using 
    int tag=*(int *)a;
    union node *client;
    if(_gain_lock_and_exec(&local_start_mtx, fcall_st, 3, client, local_start, tag))
    {
        goto exit;
    }

    //allocate command buffers
    char *cmdr=(char *)allocate("char", 512), *local_cmdr=(char *)allocate("char", 512);
    sprintf(local_cmdr, "genesis");

    //main loop
    for(local_cmdr; strcmp(local_cmdr, "END");)
    {
        deallocate(local_cmdr, "char", 512);
        deallocate(fcall_st, "struct func_call", 1);
        
        if((local_cmdr=rcv(client->local->sock, "receive from UD client", 0))==NULL)
        {
            fprintf(stderr, "\n[-]Error in reading for client with tag %s\n", tag);
            break;
        }
        
        if(!strcmp(strtok(local_cmdr, ":"), "ADD"))
        {
            //add node call
            //fcall_st init
            fcall_st=_init_fcall_st(0);
            //new node init
            union node *new=(union node *)allocate("union node", 1);
            new->nxt=NULL;
            new->prev=NULL;
            new->bmn=NULL;
            new->local=NULL;
            new->ctrlr=(struct controller *)allocate("struct controller", 1);
            new->tag=

            if(_gain_lock_and_exec(&ctrlr_start_mtx, fcall_st, 3, new, ctrlr_start, 0))
            {
                break;
            }
            deallocate(new->ctrlr, "struct controller", 1);
            deallocate(new, "union node *", 1);
        }
        else if(!strcmp(strtok(local_cmdr, ":"), "BROADCAST"))
        {
            //broadcast call
            fcall_st=_init_fcall_st(2);
            struct controller *sender;

        }
        else if(!strcmp(strtok(local_cmdr, ":"), "FOUND"))
        {
            //snd_pkt_back call
            fcall_st=_init_fcall_st(3);
        }
        
    }

exit:
    deallocate(local_cmdr, "char", 512);
    deallocate(fcall_st, "struct func_call", 1);
    close(client->local->sock);
    pthread_exit(NULL);
}

struct func_call *_init_fcall_st(int flag)
{
    struct func_call *fcall_st=(struct func_call *)allocate("struct func_call", 1);

    switch(flag)
    {
     case 0:
         fcall_st->add=add_node;
         fcall_st->find=NULL;
         fcall_st->bcast=NULL;
         fcall_st->snd_bk=NULL;
         break;
     case 1:
         fcall_st->add=NULL;
         fcall_st->find=find_node;
         fcall_st->bcast=NULL;
         fcall_st->snd_bk=NULL;
         break;
     case 2:
         fcall_st->add=NULL;
         fcall_st->find=NULL;
         fcall_st->bcast=broadcast;
         fcall_st->snd_bk=NULL;
         break;
     case 3:
         fcall_st->add=NULL;
         fcall_st->find=NULL;
         fcall_st->bcast=NULL;
         fcall_st->snd_pkt=snd_pkt_back;
    }
}

int _gain_lock_and_exec(pthread_mutex_t *lock, struct func_call *fcall_st, int num, ...)
{
    int stat, ret_val=0;
    va_list lst;
    va_start(lst, num);

    if((stat=pthread_mutex_lock(lock))!=0)
    {
        fprintf(stderr, "\n[-]Error in gaining lock: %s\n", strerror(stat));
        ret_val=1;
        goto exit;
    }

    if(fcall_st->add!=NULL)
    {
        union node *new=va_arg(lst, union node *), *start=va_arg(lst, union node *);
        int flag=va_arg(lst, int);
        fcall_st->add(new, start, flag);
        goto end;
    }
    else if(fcall_st->find!=NULL)
    {
        union node *ret=va_arg(lst, union node *), *start=va_arg(lst, union node *);
        int tag=va_arg(lst, int);

        if((ret=fcall_st->find(start, tag))==NULL)
        {
            ret_val=1;
        }
        goto end;
    }
    else if(fcall_st->bcast!=NULL)
    {
        struct controller *sender=va_arg(lst, struct controller *);
        char *cmds=va_arg(lst, char *);
        fcall_st->bcast(sender, cmds);
        goto end;
    }
    else if(fcall_st->snd_bk!=NULL)
    {
        //snd_pkt_back arguments
    }

end:
    if((stat=pthread_mutex_unlock(lock))!=0)
    {
        fprintf(stderr, "\n[-]Error in unlocking mutex: %s\n", strerror(errno));
        ret_val=1;
    }

exit:
    va_end(lst);
    return ret_val;
}
