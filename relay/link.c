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

    //copy contents
    tmp->id=id;
    tmp->done=0;
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
    }
    else
    {
        curr->nxt=tmp;
    }
    //equate current and tmp
    tmp->nxt=NULL;
    tmp->prev=curr;

    curr=tmp;
}

void del_node(int id)
{
    //init
    struct bcast_msg_node *curr=iterate(id);

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

struct bcast_msg_node *iterate(int id)
{
    struct bcast_msg_node *curr=bcast_start->nxt;

    if(id==-1)
    {
        //iterate to end;
        for(curr; curr->nxt!=NULL; curr=curr->nxt){}
    }
    else
    {
        for(curr; curr->id!=id; curr=curr->nxt){}
    }

    return curr;
}

void equate_controllers(struct controller *dest, struct controller *src)
{
    dest->bcast_sock=src->bcast_sock;
    dest->sock=src->sock;
    dest->addr.sin_port=htons(ntohs(src->addr.sin_port));
    dest->addr.sin_addr.s_addr=inet_addr(inet_ntoa(src->addr.sin_addr));
    dest->addr.sin_family=AF_INET;
}
