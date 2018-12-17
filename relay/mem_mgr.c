#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/socket.h>
#include<sys/un.h>
#include<errno.h>

#include"global_defs.h"
#include"mem_mgr.h"
#include"sock_create.h"
#include"list.h"
#include"broadcast.h"
#include"allocate.h"

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

int mem_mgr()
{
    int sock;
    if((sock=sock_create_local(".sock", 1))==-1)
    {
        return 1;
    }
    
    ctrlr_start=NULL;
    bmn_start=NULL;
    int stat;
    pthread_t tid[10];
    union node *new=(union node *)allocate("union node", 1);
    new->local =(struct local_struct *)allocate("struct local_struct", 1);
    socklen_t len=sizeof(new->local->addr);

    for(int i=0;;)
    {
        if((new->local->sock=accept(sock, (struct sockaddr *)&new->local->addr, &len))==-1)
        {
            fprintf(stderr, "\n[-]Error in accepting local connection %d: %s", i, strerror(errno));
            continue;
        }
        new->tag=i;
        add_node(local_start, new, 2);

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
    int tag=*(int *)a;
    union node *client=find_node(local_start, tag);


}
