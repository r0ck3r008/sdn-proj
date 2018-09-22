#ifndef BROADCAST_H
#define BROADCAST_H

struct broadcast_struct
{
    struct controller *cli;
    char *cmds;
};

int broadcast(struct controller *, char *);
void *broadcast_run(void *);
int send_pkt_back(struct controller *, int, char *);
void *cleanup_run(void *);

#endif
