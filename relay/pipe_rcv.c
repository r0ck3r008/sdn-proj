#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<pthread.h>
#include<errno.h>

#include"global_defs.h"
#include"pipe_rcv.h"
#include"list.h"
#include"allocate.h"

int pipefd;
union list *new;

void *pipe_rcv(void *a)
{
    char *pipe_name=(char *)a;
    start_nn=NULL;
    new=(union list *)allocate("union list", 1);
    new->nxt=NULL;
    new->prev=NULL;
    new->nn.msg=(char *)allocate("char", 50);
    char *cmdr=(char *)allocate("char", 512);
    sprintf(cmdr, "genisis");

    if((pipefd=open_pipe(pipe_name, O_RDONLY))==-1)
    {
        fprintf(stderr, "\n[-]Error in opening pipefd: %s\n", strerror(errno));
        goto exit;
    }

    for(; strcmp(cmdr, "END"); )
    {
        explicit_bzero(cmdr, sizeof(char)*512);
        if(read(pipefd, cmdr, sizeof(char)*512)==-1)
        {
            fprintf(stderr, "\n[-]Error in reading from pipefd %s\n", strerror(errno));
            break;
        }

        //add new node
        sprintf(new->nn.msg, "%s", cmdr);
        add_node(new, start_nn, 2);
        explicit_bzero(new->nn.msg, sizeof(char)*50);
    }

exit:
    explicit_bzero(cmdr, sizeof(char)*512);
    free(cmdr);
    close(pipefd);
    pthread_exit(NULL);
}

int open_pipe(char *name, int flag)
{
    int pipe;
    if((pipe=open(name, flag))==-1)
    {
        fprintf(stderr, "\n[-]Error in opening %s with flag %d: %s\n", name, flag, strerror(errno));
    }

    return pipe;
}
