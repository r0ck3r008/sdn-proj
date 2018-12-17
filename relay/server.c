#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>

#include"global_defs.h"
#include"server.h"
#include"sock_create.h"
#include"tcp_child.h"
#include"mem_mgr.h"

int server_workings(char *argv)
{
//TODO: add more parent server functionality like live monitoring or redundancy;
    pid_t main_pid=getpid();
    
    if(fork()==0)
    {
        //mem manager
        if(mem_mgr())
        {
            _exit(-1);
        }
        
    }

    if(getpid()==main_pid)
    {
        if(fork()==0)
        {
            if((tcp_sock=sock_create(argv, 1))==-1)
            {
                return 1;
            }

            //child
            tcp_child();
        }
    } 
}
