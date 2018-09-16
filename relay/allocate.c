#define NEEDS_CONTROLLER_STRUCT

#include"global_defs.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>

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

    if(ret==NULL)
    {
        fprintf(stderr, "\n[-]Error in allocating %d bytes for %s type\n", size, type);
        _exit(-1);
    }

    return ret;
}
