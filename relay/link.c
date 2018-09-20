#define NEEDS_STRUCT

#include"global_defs.h"
#include"link.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

void add_node(struct controller *cli, char *msg, int id)
{
    static struct bcast_msg_node *curr;

    //allocate spaces
    struct bcast_msg_node *tmp= (struct bcast_msg_node *)allocate("struct bcast_msg_node", 1);
    tmp->sender=(struct controller *)allocate("struct controller", 1);
    tmp->msg=(char *)allocate("char", 512);
    tmp->id=id;

    //copy contents
    sprintf(tmp->msg, "%s", msg);
    equate_controllers(tmp->sender, cli);

    if(bcast_start==NULL)
    {
        //allocate start
        bcast_start=(struct bcast_msg_node *)allocate("struct bcast_msg_node", 1);
        bcast_start->msg=(char *)allocate("char", 512);
        bcast_start->sender=(struct controller *)allocate("struct controller", 1);

        //fill start
        sprintf(bcast_start->msg, "genisis");
        bcast_start->id=-1;
        bcast_start->nxt=tmp;
        bcast_start->prev=NULL;
        bcast_start->sender=NULL;

        //connect tmp
        tmp->prev=bcast_start;
        tmp->nxt=NULL;
    }
    else
    {
        curr->nxt=tmp;
        tmp->prev=curr;
        tmp->nxt=NULL;
    }
    //equate current and tmp
    curr=tmp;
}

void del_node(int id)
{
    //init
    struct bcast_msg_node *curr=bcast_start->nxt;

    //iterate
    for(curr; curr->id!=id; curr=curr->nxt){}

    //links
    curr->prev->nxt=curr->nxt;
    if(curr->nxt!=NULL)
    {
        curr->nxt->prev=curr->prev;
    }

    //free
    free(curr->msg);
    free(curr->sender);
    free(curr);
}

void equate_controllers(struct controller *dest, struct controller *src)
{
    dest->sock=src->sock;
    dest->addr.sin_port=htons(ntohs(src->addr.sin_port));
    dest->addr.sin_addr.s_addr=inet_addr(inet_ntoa(src->addr.sin_addr));
    dest->addr.sin_family=AF_INET;
}
