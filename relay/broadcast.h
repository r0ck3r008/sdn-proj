#ifndef BROADCAST_H
#define BROADCAST_H

int broadcast(struct controller *, char *, pthread_mutex_t *);
void broadcast_run(union node *, char *);
void *cleanup_run(void *);
int send_pkt_back(struct controller *, char *, pthread_mutex_t *);

#endif
