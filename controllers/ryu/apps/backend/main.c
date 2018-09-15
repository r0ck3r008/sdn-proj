#include"sock_create.h"
#include"dbconnect.h"

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

int init(int argc)
{
    if(argc!=3)
    {
        fprintf(stderr, "\n[!]Usage: ./backend [ip_to_bin:port_to_bind] [dbinterface_server_ip:dbinterface_server_port]\n");
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

    if(sock_create(argv[1]))
    {
        _exit(-1);
    }

    if(dbconnect(argv[2], 0))
    {
        _exit(-1);
    }
}
