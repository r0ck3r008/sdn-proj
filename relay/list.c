#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include"global_defs.h"
#include"list.h"
#include"allocate.h"


void add_node(union list *new, union list *start, int fl)//can be 0(for bcast) or 2(for normie)
{
    static union list *curr_nn, *curr_bmn;
    union list *tmp;

    if(fl)
    {
        equate(tmp, new, (0+fl));
    }
    else
    {
        equate(tmp, new, (0+fl));
    }

    if(start==NULL)
    {
        equate(start, tmp, (1+fl));
        tmp->prev=start;
    }
    else
    {
        if(fl)
        {
            curr_nn->nxt=tmp;
            tmp->prev=curr_nn;
            tmp->nxt=NULL;
            curr_nn=tmp;
        }
        else
        {
            curr_bmn->nxt=tmp;
            tmp->prev=curr_bmn;
            tmp->nxt=NULL;
            curr_bmn=tmp;
        }
    }
}

void equate(union list *a, union list *b, int fl)// 1 for start/node, 2 for normie/bcast
{
    a=(union list *)allocate("union list", 1);

    if(fl==(1+2))//normie start
    {
//        a=(union list *)allocate("union list", 1);
        a->nn.msg=NULL;
        a->prev=NULL;
        a->nxt=b;
    }
    else if(fl==(1+0))//bcast start
    {
        a->bmn.id=-1;
        a->bmn.done=-1;
        a->bmn.sender=NULL;
        a->bmn.msg=NULL;
        a->prev=NULL;
        a->nxt=b;
    }
    else if(fl==(0+2))//normie node
    {
        a->nn.msg=(char *)allocate("char", 50);

        sprintf(a->nn.msg, "%s", b->nn.msg);
    }
    else if(fl==(0+0))//bcast node
    {
        a->bmn.msg=(char *)allocate("char", 50);
        a->bmn.sender=(struct controller *)allocate("struct controller", 1);
       
        a->bmn.id=b->bmn.id;
        a->bmn.done=b->bmn.done;
        sprintf(a->bmn.msg, "%s", b->bmn.msg);
        a->bmn.sender->sock=b->bmn.sender->sock;
        a->bmn.sender->addr.sin_family=AF_INET;
        a->bmn.sender->addr.sin_port=b->bmn.sender->addr.sin_port;
        a->bmn.sender->addr.sin_addr.s_addr=inet_addr(inet_ntoa(b->bmn.sender->addr.sin_addr));
    }
}
