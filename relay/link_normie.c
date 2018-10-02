#define NEEDS_NORMIE

#include"global_defs.h"
#include"link_normie.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>


void add_node(char *msg)
{
    static struct normie_node *curr;
    struct normie_node *tmp=(struct normie_node *)allocate("struct normie_node", 1);
    tmp->msg=(char *)allocate("char", 50);
    sprintf(tmp->msg, "%s", msg);

    if(normie_start==NULL)
    {
        //init start
        normie_start=(struct normie_node *)allocate("struct normie_node", 1);
        normie_start->msg=(char *)allocate("char", 50);
        sprintf(normie_start->msg, "genisis");
        normie_start->prev=NULL;
        normie_start->nxt=tmp;
        tmp->nxt=NULL;
        tmp->prev=normie_start;
    }
    else
    {
        curr->nxt=tmp;
        tmp->prev=curr;
    }
    curr=tmp;
}

void del_node(char *msg)
{
    struct normie_node *curr=normie_start->nxt;

    for(curr; !strcmp(curr->msg, msg); curr=curr->nxt){}

    curr->prev->nxt=curr->nxt;
    if(curr->nxt!=NULL)
    {
        curr->nxt->prev=curr->prev;
    }


    free(curr->msg);
    free(curr);
}

int find(char *msg)
{
    int ret=0;
    struct normie_node *curr=normie_start->nxt;

    for(curr; curr!=NULL; curr=curr->nxt)
    {
        if(!strcmp(curr->msg, msg))
        {
            ret=1;
            break;
        }
    }

    return ret;
}
