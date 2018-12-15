#include<stdio.h>
#include<string.h>
#include<unistd.h>

#include"global_defs.h"
#include"server.h"


int init(int argc)
{
    if(argc!=2)
    {
        fprintf(stderr, "\n[!]Usage: ./relay [ip_for_tcp]\n");
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

    if(server_workings(argv[1])==-1)
    {
        _exit(-1);
    }
}
