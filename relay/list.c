#define NEEDS_STRUCT

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"global_defs.h"
#include"list.h"
#include"allocate.h"

void add_node(union node *new, union node *start, int flag)//2 for ctrlr, 4 for bmn
{
    union node *tmp;
    static union node *curr_ctrlr, *curr_bmn;
    
    gen_equate(tmp, new, 1+flag);
}

void gen_equate(union node *a, union node *b, int flag)//0 for start, 1 for tmp
{
    a=(union node *)allocate("union node", 1);

    switch(flag)
    {
 case 2://start of ctrlr
    {
        a->ctrlr->id=-1;
        a->ctrlr->bcast_sock=-1;
        a->ctrlr->sock=-1;
//        a->ctrlr->addr

        break;
    }
 case 3://tmp for ctrlr
    {
        break;
    }
 case 4://start for bmn
    {
        break;
    }
 case 5://tmp for bmn
    {
        break;
    }
 default:
    {
        
    }
    }
}

union node *find_node(union node *start, int tag)
{
    union node *curr;
    for(curr=start->nxt; curr->tag!=tag || curr->nxt!=NULL; curr=curr->nxt);
    if(curr==NULL)
    {
        fprintf(stderr, "\n[-]Item not found with tag %d\n", tag);
    }
    return curr;
}

int del_node(union node *start, int tag)
{
    union node *curr;
    if((curr=find_node(start, tag))!=NULL)
    {
        curr->prev->nxt=curr->nxt;
        if(curr->nxt!=NULL)
        {
            curr->nxt->prev=curr->prev;
        }

        explicit_bzero(curr, sizeof(union node));
        free(curr);
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
