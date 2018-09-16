#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

    #ifdef NEEDS_CONTROLLER_STRUCT
        #include<sys/socket.h>
        #include<netinet/in.h>
        #include<arpa/inet.h>
        struct controller
        {
            int sock;
            struct sockaddr_in addr;
        } *cli;
    #endif


int server_sock;

#endif
