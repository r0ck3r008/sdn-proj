#define NEEDS_ALL

#include"global_defs.h"
#include"broadcast.h"
#include"allocate.h"
#include"snd_rcv.h"
#include"link.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sodium.h>
#include<errno.h>

//writer
int broadcast(struct controller *sender, char *cmds)
{
    //declarations
    int stat;
    uint32_t *rand=(uint32_t *)allocate("uint32_t", 1), *max=(uint32_t *)allocate("uint32_t", 1);
    *max=1000000000;
    *rand=randombytes_uniform(*max);
    pthread_t tid[cli_num-1];
    struct broadcast_struct *b_struct=(struct broadcast_struct *)allocate("struct broadcast_struct", cli_num-1);
    for(int i=0; i<cli_num-1; i++)
    {
        b_struct[i].cmds=(char *)allocate("char", 512);
        b_struct[i].cli=(struct controller *)allocate("struct controller", 1);
    }

    if((stat=pthread_mutex_lock(&speaker))!=0)
    {
        fprintf(stderr, "\n[-]Error in locking mutex for client %s:%s: %s\n", inet_ntoa(sender->addr.sin_addr), ntohs(sender->addr.sin_port), strerror(stat));
        return 1;
    }

    for(int i=0; i<cli_num; i++)
    {
        static int j=0;
        if(strcmp(inet_ntoa(sender->addr.sin_addr), inet_ntoa(cli[i].addr.sin_addr)))
        {
            j++;
            ///equate b_struct
            b_struct[j].cli->sock=cli[i].sock;
            b_struct[j].cli->addr=cli[i].addr;
            sprintf(b_struct[j].cmds, "%s", cmds);

            //send the packet
            if((stat=pthread_create(&tid[j], NULL, broadcast_run, &b_struct[j]))!=0)
            {
                fprintf(stderr, "\n[-]Error in broadcasting %s to %s: %s\n", cmds, inet_ntoa(cli[i].addr.sin_addr), strerror(stat));
                continue;
            }

            if((stat=pthread_join(tid[j], NULL))!=0)
            {
                fprintf(stderr, "\n[-]Error in joining broadcast thr back for %s: %s\n", inet_ntoa(cli[i].addr.sin_addr), strerror(stat));
                continue;
            }
        }
    }
    add_node(sender, cmds, *rand);


    if((stat=pthread_mutex_unlock(&speaker))!=0)
    {
        fprintf(stderr, "\n[-]Error in unlocking mutex for client %s:%d: %s\n", inet_ntoa(sender->addr.sin_addr), ntohs(sender->addr.sin_port), strerror(stat));
        return 1;
    }

    for(int i=0; i<cli_num-1; i++)
    {
        free(b_struct[i].cmds);
        free(b_struct[i].cli);
        free(&b_struct[i]);
    }

    free(max);
    free(rand);
    return 0;
}

void *broadcast_run(void *a)
{
    struct broadcast_struct *b_struct=(struct broadcast_struct *)a;

    int ret=snd(b_struct->cli, b_struct->cmds, "broadcast msg", NULL, 0);
    pthread_exit(NULL);
}
