#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/un.h>

#include"global_defs.h"
#include"list.h"
#include"allocate.h"

void add_node(union node *new, union node *start, int flag)//0 for ctrlr, 1 for bmn
{
    union node *tmp;
    static union node *curr[2];//0 for ctrlr, 1 for bmn
    
    _general_equate(tmp, new, (4+flag));

    if(start==NULL)
    {
        _general_equate(start, tmp, 2);
        start->nxt=tmp;
        tmp->prev=start;
    }
    else
    {
        curr[flag]->nxt=tmp;
        tmp->prev=curr[flag];
    }
    tmp->nxt=NULL;
    curr[flag]=tmp;
}

void _general_equate(union node *a, union node *b, int flag)//0 for start, 1 for tmp
{
    a=(union node *)allocate("union node", 1);

    switch(flag)
    {
     case 2:
        _general_equate_start(a, b);
        break;

     case (4+0)://tmp for ctrlr
        _equate_ctrlr(a, b);
        break;

     case (4+1)://tmp for bmn
        _equate_bmn(a, b);
    }
}

void _general_equate_start(union node *a, union node *b)
{
    a->prev=NULL;
    a->nxt=NULL;
    a->tag=-1;
    a->ctrlr=NULL;
    a->bmn=NULL;
    a=b;
}

void _equate_ctrlr(union node *a, union node *b)
{
    a->tag=b->tag;
    a->ctrlr=(struct controller *)allocate("struct controller", 1);
    a->ctrlr->bcast_sock=b->ctrlr->bcast_sock;
    a->ctrlr->sock=b->ctrlr->sock;
    memcpy(&a->ctrlr->addr, &b->ctrlr->addr, sizeof(struct sockaddr_in));
}

void _equate_bmn(union node *a, union node *b)
{
    a->tag=b->tag;
    a->bmn=(struct bcast_msg_node *)allocate("struct bcast_msg_node", 1);
    a->bmn->done=0;
    //msg
    a->bmn->msg=(char *)allocate("char", 512);
    sprintf(a->bmn->msg, "%s", b->bmn->msg);
    //controller
    a->bmn->sender=b->bmn->sender;
}

union node *find_node(union node *start, int tag)
{
    union node *curr;
    if(start==NULL)
    {
        fprintf(stderr, "\n[-]Empty list passed to find_node\n");
    }
    for(curr=start->nxt; curr->tag!=tag || curr->nxt!=NULL; curr=curr->nxt);
    if(curr==NULL)
    {
        fprintf(stderr, "\n[-]Item not found with tag %d\n", tag);
    }
exit:
    return curr;
}

int del_node(union node *start, int flag, int tag)
{
    union node *curr;
    if((curr=find_node(start, tag))!=NULL)
    {
        curr->prev->nxt=curr->nxt;
        if(curr->nxt!=NULL)
        {
            curr->nxt->prev=curr->prev;
        }

        if(!flag)//ctrlr
        {
            deallocate(curr->ctrlr, "struct controller", 1);
            deallocate(curr, "union node", 1);
        }
        else//bmn
        {
            deallocate(curr->bmn->msg, "char", 512);
            deallocate(curr->bmn, "struct bcast_msg_node", 1);
            deallocate(curr, "union node", 1);
        }
    }
    else
    {
        fprintf(stderr, "\n[-]Cant delete item with tag %d\n", tag);
        return -1;
    }
}

int list_len(union node *start)
{
    if(start==NULL)
    {
        fprintf(stderr, "\n[-]Empty list passed\n");
        return -1;
    }

    union node *curr=start->nxt;
    int i;
    for(i=0; curr->nxt!=NULL; curr=curr->nxt)
    {
        i++;
    }
    return i;
}
