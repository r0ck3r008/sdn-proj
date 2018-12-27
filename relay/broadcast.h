#ifndef BROADCAST_H
#define BROADCAST_H

int broadcast(struct controller *, char *, struct mutex_call *, uint32_t *);
int _broadcast_helper(struct mutex_call *, struct controller *, char *, int *, uint32_t *);
void _broadcast_run(struct controller *, char *);
int send_pkt_back(struct controller *, char *, struct mutex_call *);
int _del_bmn(struct mutex_call *, uint32_t *);

#endif
