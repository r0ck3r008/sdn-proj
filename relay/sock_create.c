#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/un.h>
#include<sys/types.h>
#include<unistd.h>
#include<errno.h>

#include"sock_create.h"

int sock_create(char *addr, int flag)
{
    int s;
    struct sockaddr_in addr_st;
    explicit_bzero(&addr_st, sizeof(struct sockaddr_in));
    addr_st.sin_family=AF_INET;
    addr_st.sin_addr.s_addr=inet_addr(addr);

    if((s=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in creating socket for addr %s: %s\n", addr, strerror(errno));
        goto exit;
    }

    if(flag)//server
    {
        addr_st.sin_port=htons(12345);
        if(bind(s, (struct sockaddr *)&addr_st, sizeof(addr_st))==-1)
        {
            fprintf(stderr, "\n[-]Error in binding address %s: %s\n", addr, strerror(errno));
            s=-1;
            goto exit;
        }
        if(listen(s, 5)==-1)
        {
            fprintf(stderr, "\n[-]Error in listning on %s: %s\n", addr, strerror(errno));
            s=-1;
        }
    }
    else
    {
        addr_st.sin_port=htons(6666);
        if(connect(s, (struct sockaddr *)&addr_st, sizeof(addr_st))==-1)
        {
            fprintf(stderr, "\n[-]Error in connecting to %s at 6666: %s\n", addr, strerror(errno));
            s=-1;
        }
    }

exit:
    return s;
}

int sock_create_local(char *addr, int flag)
{
    int sock;
    if((sock=socket(AF_LOCAL, SOCK_STREAM, 0))==-1)
    {
        fprintf(stderr, "\n[-]Error in creating local socket for addr %s: %s\n", addr, strerror(errno));
        goto exit;
    }

    unlink(addr);

    struct sockaddr_un addr_st;
    explicit_bzero(&addr, sizeof(struct sockaddr_un));
    addr_st.sun_family=AF_LOCAL;
    memcpy(addr_st.sun_path, addr, strlen(addr));

    if(flag)//server
    {
        if(bind(sock, (struct sockaddr *)&addr_st, SUN_LEN(&addr_st))==-1)
        {
            fprintf(stderr, "\n[-]Error in binding to address %s, local socket: %s\n", addr, strerror(errno));
            close(sock);
            sock=-1;
            goto exit;
        }

        if(listen(sock, 5)==-1)
        {
            fprintf(stderr, "\n[-]Error in listning to local socket: %s\n", strerror(errno));
            close(sock);
            sock=-1;
            goto exit;
        }

        printf("\n[!]Local socket successfully bound and listning\n");
    }
    else//client
    {
        if(connect(sock, (struct sockaddr *)&addr_st, SUN_LEN(&addr_st))==-1)
        {
            fprintf(stderr, "\n[-]Error in connecting to local server at %s: %s\n", addr, strerror(errno));
            close(sock);
            sock=-1;
            goto exit;
        }
        printf("\n[!]Connected to addr at %s\n", addr);
    }

    exit:
    return sock;
}
