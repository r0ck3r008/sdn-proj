#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

int server_sock;

    #ifdef NEEDS_ALL
        #define NEEDS_STRUCT
        #define NEEDS_MUTEX
    #endif

    #ifdef NEEDS_STRUCT
        #include<sys/socket.h>
        #include<netinet/in.h>
        #include<arpa/inet.h>
        struct controller
        {
            int sock;
            struct sockaddr_in addr;
        } *cli;
        
        struct bcast_msg_node
        {
            int id;
            char *msg;
            struct controller *sender;
            struct bcast_msg_node *nxt;
            struct bcast_msg_node *prev;
        } *bcast_start=NULL;

    #endif

    #ifdef NEEDS_MUTEX
        #include<pthread.h>
        pthread_mutex_t speaker=PTHREAD_MUTEX_INITIALIZER;
    #endif

#endif
