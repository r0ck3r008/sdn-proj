#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

int udp_sock, tcp_sock;
int cli_num;
int *done_bcast_nodes;

    #ifdef NEEDS_ALL
        #define NEEDS_STRUCT
        #define NEEDS_MUTEX
    #endif

    #ifdef NEEDS_STRUCT

        struct controller
        {
            int bcast_sock, sock;
            struct sockaddr_in addr;
        } *cli;
        
        struct bcast_msg_node
        {
            int id;
            int done;
            char *msg;
            struct controller *sender;
            struct bcast_msg_node *prev;
            struct bcast_msg_node *nxt;
        } *bcast_start;

        struct normie_node
        {
            char *msg;
            struct normie_node *nxt;
            struct normie_node *prev;
        } *normie_start;

        union list
        {
            struct normie_node n, *nn, *pn;
            struct bcast_msg_node bmn, *nbmn, *pbmn;
        } *startnn, *startbmn;

    #endif

    #ifdef NEEDS_MUTEX

        pthread_mutex_t speaker=PTHREAD_MUTEX_INITIALIZER;

    #endif

#endif
