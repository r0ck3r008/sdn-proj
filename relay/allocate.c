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
    else if(!strcmp(type, "uint32_t"))
    {
        ret=malloc(sizeof(uint32_t)*size);
        explicit_bzero(ret, sizeof(uint32_t)*size);
    }
    else if(!strcmp(type, "union list"))
    {
        ret=malloc(sizeof(union list)*size);
        explicit_bzero(ret, sizeof(union list)*size);
    }

    if(ret==NULL)
    {
        fprintf(stderr, "\n[-]Error in allocating %d bytes for %s type\n", size, type);
        _exit(-1);
    }

    return ret;
}
