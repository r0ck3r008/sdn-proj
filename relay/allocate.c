#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include"global_defs.h"
#include"allocate.h"
#include"broadcast.h"
#include"lock_and_exec.h"


void *allocate(char *type, int size)
{
    void *ret=NULL;

    if(!strcmp(type, "char"))
    {
        ret=malloc(sizeof(char)*size);
        explicit_bzero(ret, sizeof(char)*size);
    }
    else if(!strcmp(type, "struct controller"))
    {
        ret=malloc(sizeof(struct controller)*size);
        explicit_bzero(ret, sizeof(struct controller)*size);
    }
    else if(!strcmp(type, "struct bcast_msg_node"))
    {
        ret=malloc(sizeof(struct bcast_msg_node)*size);
        explicit_bzero(ret, sizeof(struct bcast_msg_node)*size);
    }
    else if(!strcmp(type, "union node"))
    {
        ret=malloc(sizeof(union node)*size);
        explicit_bzero(ret, sizeof(union node)*size);
    }
    else if(!strcmp(type, "struct func_call"))
    {
        ret=malloc(sizeof(struct func_call)*size);
        explicit_bzero(ret, sizeof(struct func_call)*size);
    }

    if(ret==NULL)
    {
        fprintf(stderr, "\n[-]Error in allocating %d bytes for %s type\n", size, type);
        _exit(-1);
    }

    return ret;
}

void deallocate(void *a, char *type, int size)
{
    if(!strcmp(type, "char"))
    {
        explicit_bzero(a, sizeof(char)*size);
    }
    else if(!strcmp(type, "struct controller"))
    {
        explicit_bzero(a, sizeof(struct controller)*size);
    }
    else if(!strcmp(type, "struct bcast_msg_node"))
    {
        explicit_bzero(a, sizeof(struct bcast_msg_node)*size);
    }
    else if(!strcmp(type, "union node"))
    {
        explicit_bzero(a, sizeof(union node)*size);
    }
    else if(!strcmp(type, "struct func_call"))
    {
        explicit_bzero(a, sizeof(struct func_call)*size);
    }

    free(a);
}
