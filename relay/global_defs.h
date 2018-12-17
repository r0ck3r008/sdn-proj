#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

int tcp_sock;
int *done_bcast_nodes;

    #ifdef NEEDS_STRUCT

        struct controller
        {
            int bcast_sock, sock;
            struct sockaddr_in addr;
        };
        
        struct bcast_msg_node
        {
            int done;
            char *msg;
            struct controller *sender;
        };

        struct local_struct
        {
            int sock;
            struct sockaddr_un addr;
        }

        union node
        {
            int tag;
            struct controller *ctrlr;
            struct bcast_msg_node *bmn;
            struct local_struct *local;
            union node *nxt;
            union node *prev;
        } *ctrlr_start, *bmn_start, *local_start;

    #endif
#endif
