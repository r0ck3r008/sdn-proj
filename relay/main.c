#include<stdio.h>
#include<unistd.h>

int init(int argc)
{
    if(argc!=2)
    {
        fprintf(stderr, "\n[!]Usage: ./relay [ip_to_bind:port_to_bind]\n");
        return 1;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if(init(argc))
    {
        _exit(-1);
    }
}
