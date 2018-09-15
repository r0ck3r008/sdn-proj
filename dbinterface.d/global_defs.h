#ifndef GLOBAL_DEFS_H
#define GLOBAL_DEFS_H

int server_sock;

    #ifdef NEEDS_ALL
        #define NEEDS_CLIENT_STRUCT
        #define NEEDS_DB
        #define NEEDS_MUTEX
    #endif

    #ifdef NEEDS_CLIENT_STRUCT
    #include<sys/socket.h>
    #include<netinet/in.h>
    #include<arpa/inet.h>
    struct client
    {
        struct sockaddr_in cli;
        char *msg;
    };
    #endif

    #ifdef NEEDS_DB
        #include<mysql/mysql.h>
        #include<termios.h>
        MYSQL *conn;
        MYSQL_RES *res;
        MYSQL_ROW row;
    #endif

    #ifdef NEEDS_MUTEX
        #include<pthread.h>
        extern pthread_mutex_t mutex;
    #endif

#endif
