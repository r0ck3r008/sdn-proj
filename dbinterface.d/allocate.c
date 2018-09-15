#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>

void *allocate(char *type, int size)
{
    void *ret;

    if(strcmp(type, "char")==0)
    {
        ret=malloc(size*sizeof(char));
        explicit_bzero(ret, sizeof(char)*size);
    }

    if(ret==NULL)
    {
        fprintf(stderr, "\n[-]Error in allocating %d bytes for %s type\n", type, size);
        return NULL;
    }

    return ret;
}
