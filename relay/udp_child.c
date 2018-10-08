#define NEEDS_STRUCT
#define NEEDS_NORMIE

#include"global_defs.h"
#include"udp_child.h"
#include"snd_rcv.h"
#include"link_normie.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>

int udp_child()
{
    normie_start=NULL;
    int ret=0, stat;
    char *cmdr=(char *)allocate("char", 512), *addr=(char *)allocate("char", 50);
    sprintf(cmdr, "genisis");
    sprintf(addr, "genisis");
    
    for(int i=0; ;)
    {
        free(cmdr);
        free(addr);

        char *addr=(char *)allocate("char", 50);
        if((cmdr=rcv_frm(addr))==NULL)
        {
            fprintf(stderr, "\n[-]Error in receving: %d\n", i);
            ret=1;
            break;
        }
        add_node(addr);

        i++;
    }

    return ret;
}
