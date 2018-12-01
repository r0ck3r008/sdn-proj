#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

int tcp_sock;
int cli_num;
int *done_bcast_nodes;

    #ifdef NEEDS_ALL
        #define NEEDS_STRUCT
        #define NEEDS_MUTEX
    #endif

    #ifdef NEEDS_STRUCT

        struct controller
        {
            int id;
            int bcast_sock, sock;
            struct sockaddr_in addr;
        };
        
        struct bcast_msg_node
        {
            int id;
            int done;
            char *msg;
            struct controller *sender;
        };

        union node
        {
            int tag;
            struct controller *ctrlr;
            struct bcast_msg_node *bmn;
            union node *nxt;
            union node *prev;
        } *ctrlr_start, *bmn_start;

    #endif

    #ifdef NEEDS_MUTEX

        pthread_mutex_t speaker=PTHREAD_MUTEX_INITIALIZER;

    #endif

#endif
