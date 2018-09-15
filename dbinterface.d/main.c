#define NEEDS_ALL

#include"global_defs.h"
#include"dbconnect.h"
#include"server.h"

#include<stdio.h>
#include<unistd.h>

pthread_mutex_t muex=PTHREAD_MUTEX_INITIALIZER;

int init(int argc)
{
    if(argc!=3)
    {
        fprintf(stderr, "\n[!]Usage: ./dbinterface [ip_to_bind:port_to_bind] [db_server:db_uname:db_name]\n");
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

    if(server_init(argv[1]))
    {
        _exit(-1);
    }

    if(dbconnect(argv[2]))
    {
        _exit(-1);
    }

    server_workings();
}
