#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

int udp_sock, tcp_sock;
int cli_num;
int *done_bcast_nodes;

    #ifdef NEEDS_ALL
        #define NEEDS_STRUCT
        #define NEEDS_MUTEX
    #endif

    #ifdef NEEDS_NORMIE
        struct normie_node
        {
            char *msg;
            struct normie_node *nxt;
            struct normie_node *prev;
        } *normie_start;
    #endif
    #ifdef NEEDS_STRUCT
        #include<sys/socket.h>
        #include<netinet/in.h>
        #include<arpa/inet.h>
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

    #endif

    #ifdef NEEDS_MUTEX
        #include<pthread.h>
        pthread_mutex_t speaker=PTHREAD_MUTEX_INITIALIZER;
    #endif

#endif
