#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<errno.h>

#include"tcp_connector.h"
#include"allocate.h"
#include"snd_rcv.h"

int get_connection_back(int sock)
{
    socklen_t len=sizeof(struct sockaddr_in);
    struct sockaddr_in addr;
    int s;
    
    if((s=accept(sock, (struct sockaddr *)&addr, &len))==-1)
    {
        fprintf(stderr, "\n[-]Error in receving back the connection: %s\n", strerror(errno));
        goto exit;
    }

    printf("\n[!]Got a connection back from %s:%d\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

exit:
    return s;
}
