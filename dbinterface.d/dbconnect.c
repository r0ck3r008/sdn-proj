#include"dbconnect.h"
#include"allocate.h"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>

char *get_passwd()
{
    char *passwd=(char *)allocate("char", 64);
    struct termios a, b;

    explicit_bzero(&a, sizeof(struct termios));
    explicit_bzero(&b, sizeof(struct termios));

    if(tcgetattr(fileno(stdin), &a)==-1)
    {
        fprintf(stderr, "\n[-]Error in getting stdin attributes: %s\n", strerror(errno));
        return NULL;
    }

    b=a;
    a.c_lflag &= ~ECHO;
    a.c_lflag |= ECHONL;

    if(tcsetattr(fileno(stdin), TCSANOW, &a)==-1)
    {
        fprintf(stderr, "\n[-]Error in setting silent attr in stdin: %s\n", strerror(errno));
        return NULL;
    }

    fgets(passwd, sizeof(char)*64, stdin);

    if(tcsetattr(fileno(stdin), TCSANOW, &b)==-1)
    {
        fprintf(stderr, "\n[-]Error in setting stdin back to normal: %s\n", strerror(errno));
        return NULL;
    }

    return passwd;
}

int dbconnect(char *argv2)
{
    char *server=strtok(argv2, ":"), *uname=strtok(NULL, ":"), *passwd, *db_name=strtok(NULL, ":");

    printf("\n[>]Enter db passwd: ");
    if((passwd=get_passwd())==NULL)
    {
        return 1;
    }

    if((conn=mysql_init(NULL))==NULL)
    {
        fprintf(stderr, "\n[-]Error in initiation conn datastructure\n");
        return 1;
    }

    if(!mysql_real_connect(conn, server, uname, passwd, db_name, 0, NULL, 0))
    {
        fprintf(stderr, "\n[-]Error in opening mysql connection: %s\n", mysql_error(conn));
        return 1;
    }

    free(passwd);
    return 0;
}

int query_db(struct client *cli, char *cmds, int privilage)
{
    int stat, exp_col, ret;
    char *query=(char *)allocate("char", 128);

    if(privilage)
    {
        sprintf(query, "insert into controllers values ('%s');", inet_ntoa(cli->cli.sin_addr));
        exp_col=0;
    }

    if((stat=pthread_mutex_lock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in locking mutex for %s: %s\n", inet_ntoa(cli->cli.sin_addr), strerror(errno));
        ret=1;
    }

    if(mysql_query(conn, query))
    {
        fprintf(stderr, "\n[-]Error in querying the db for %s: %s\n", inet_ntoa(cli->cli.sin_addr), mysql_error(conn));
        sprintf(cmds, "query fail");
        goto unlock_mutex;
    }
    res=mysql_use_result(conn);
    row=mysql_fetch_row(res);
    for(int i=0; i<exp_col; i++)
    {
        sprintf(cmds, row[i]);
    }

    if(mysql_affected_rows(conn)==1)
    {
        sprintf(cmds, "OK");
    }

    unlock_mutex:
    if((stat=pthread_mutex_unlock(&mutex))!=0)
    {
        fprintf(stderr, "\n[-]Error in unlocking mutex for %s: %s\n", inet_ntoa(cli->cli.sin_addr), strerror(errno));
        ret=1;
    }

    free(query);
    return ret;
}
