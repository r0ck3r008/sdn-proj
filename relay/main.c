#define NEEDS_STRUCT

#include"global_defs.h"
#include"server.h"

#include<stdio.h>
#include<string.h>
#include<unistd.h>

int init(int argc)
{
    if(argc!=3)
    {
        fprintf(stderr, "\n[!]Usage: ./relay [ip_to_bind:port_to_bind] [path_to_publish]\n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(init(argc))
    {
        _exit(-1);
    }

    if(create_sock(argv[1]))
    {
        _exit(-1);
    }

    if(server_workings(argv[2])==-1)
    {
        _exit(-1);
    }
}