#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<signal.h>
#include<errno.h>

#include"global_defs.h"
#include"udp_child.h"
#include"snd_rcv.h"
#include"pipe_rcv.h"
#include"allocate.h"

int pipefd;
void udp_child()
{
    if((pipefd=open_pipe(O_RDWR))==-1)
    {
        sleep(5);
    }

    int  num=1;
    a=&num;
    char *cmdr=(char *)allocate("char", 512), *addr=(char *)allocate("char", 50);
    sprintf(cmdr, "genisis");
    sprintf(addr, "genisis");

    //signal
    struct sigaction action;
    explicit_bzero(&action, sizeof(struct sigaction));
    action.sa_handler=termination;
    sigaction(SIGTERM, &action, NULL);
    
    for(int i=0; *a;)
    {
        free(cmdr);
        free(addr);

        char *addr=(char *)allocate("char", 50);
        if((cmdr=rcv_frm(addr))==NULL)
        {
            fprintf(stderr, "\n[-]Error in receving: %d\n", i);
            break;
        }
        sprintf(cmdr, "%s:%s", cmdr, addr);
        
        if(write(pipefd, cmdr, sizeof(char)*512)==-1)
        {
            fprintf(stderr, "\n[-]Error in writing %s to pipe: %s\n", cmdr, strerror(errno));
            break;
        }
        i++;
    }

}

void termination(int sig)
{
    printf("\n[!]Caught SIGNAL: %d. Exiting...\n", sig);
    char *end="END";
    if(write(pipefd, end, strlen(end))==-1)
    {
        fprintf(stderr, "\n[-]Error in ending pipe relation: %s\n", strerror(errno));
    }

    *a=0;
    close(pipefd);
}
