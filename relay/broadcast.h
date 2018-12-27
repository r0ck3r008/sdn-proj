#ifndef BROADCAST_H
#define BROADCAST_H

int broadcast(struct controller *, char *, struct mutex_call *);
int _broadcast_helper(struct mutex_call *, struct controller *, char *, int *);
void _broadcast_run(struct controller *, char *);
void *cleanup_run(void *);
int send_pkt_back(struct controller *, char *, struct mutex_call *);

#endif
