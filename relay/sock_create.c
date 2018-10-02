#include"sock_create.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

//weight of udp is 2 server is 1, ie: 0 is tcp-client, 1 is tcp server, 2 is usp client, 3 is udp server
int sock_create(char *argv, int udp_server)
{
    int s, ret=0;
    struct sockaddr_in addr;
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr=inet_addr(argv);
    
    switch(udp_server)
    {
     case 0://tcp client
         if((s=socket(AF_INET, SOCK_STREAM, 0))==-1)
         {
             fprintf(stderr, "\n[-]Error in creating tcp-client socket: %s\n", strerror(errno));
             ret=-1;
             break;
         }
         addr.sin_port=htons(6666);
         if(connect(s, (struct sockaddr *)&addr, sizeof(addr))==-1)
         {
             fprintf(stderr, "\n[-]Error in connecting tcp-client socket: %s\n", strerror(errno));
             ret=-1;
             break;
         }
         printf("\n[!]Connection established to %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
         break;

     case 1://tcp-server
         if((s=socket(AF_INET, SOCK_STREAM, 0))==-1)
         {
             fprintf(stderr, "\n[-]Error in creating tcp-server sock: %s\n", strerror(errno));
             ret=-1;
             break;
         }
         addr.sin_port=htons(12346);
         if(bind(s, (struct sockaddr *)&addr, sizeof(addr))==-1)
         {
             fprintf(stderr, "\n[-]Error in binding for tcp-server: %s\n", strerror(errno));
             ret=-1;
             break;
         }
         if(listen(s, 5)==-1)
         {
             fprintf(stderr, "\n[-]Error in listning to tcp-server: %s\n", strerror(errno));
             ret=-1;
             break;
         }
         printf("\n[!]Tcp-server bound and listning!\n");
         break;

     case 3://udp server
         if((s=socket(AF_INET, SOCK_DGRAM, 0))==-1)
         {
             fprintf(stderr, "\n[-]Error in creating udp server sock: %s\n", strerror(errno));
             ret=-1;
             break;
         }
         addr.sin_port=htons(12345);
         if(bind(s, (struct sockaddr *)&addr, sizeof(addr))==-1)
         {
             fprintf(stderr, "\n[-]Error in binding udp-server socket: %s\n", strerror(errno));
             ret=-1;
             break;
         }
         printf("\n[!]Udp-server socket bound successfully\n");
         break;

     default:
         fprintf(stderr, "\n[!]Invalid args!!\n");
         break;
    }

    if(!ret)
    {
        return s;
    }
    return ret;
}

