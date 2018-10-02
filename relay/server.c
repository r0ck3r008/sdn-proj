#include"tl_functions.h"
#include"server.h"

#include<stdio.h>

int server_workings(char *argv1, char *argv2)
{
    //udp server
    if(udp_server(argv1))
    {
        return 1;
    }

    if(tcp_server(argv2))
    {
        return 1;
    }
}
