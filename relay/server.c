#include<stdio.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>

#include"global_defs.h"
#include"server.h"
#include"tcp_child.h"
#include"sock_create.h"

int server_workings(char *addr)
{
    pid_t tcp_pid=fork();

    switch(tcp_pid)
    {
     case 0:
         //child
         tcp_sock=0;
         if((tcp_sock=sock_create(addr, 1))==-1)
         {
             fprintf(stderr, "\n[-]Exiting tcp_server...\n");
             _exit(-1);
         }
         if(tcp_child())
         {
             fprintf(stderr, "\n[-]Exiting the tcp_server...\n");
             _exit(-1);
         }
         break;

     case -1:
         fprintf(stderr, "\n[-]Error in forking off the tcp_child: %s\n", strerror(errno));
         _exit(-1);

     default:
         while(1)
         {
             if(wait(NULL)==tcp_pid)
             {
                 printf("\n[!]Closing all\n");
                 break;
             }
         }
    }
}
