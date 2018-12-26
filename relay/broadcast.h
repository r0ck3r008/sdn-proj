#ifndef BROADCAST_H
#define BROADCAST_H

int broadcast(struct controller *, char *, struct mutex_call *);
void _broadcast_run(struct controller *, char *);
void *cleanup_run(void *);
int send_pkt_back(struct controller *, char *, struct mutex_call *);

#endif
