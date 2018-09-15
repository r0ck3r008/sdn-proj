#ifndef SND_RCV_H 
#define SND_RCV_H

int snd(struct sockaddr_in, char *, char *);
char *rcv(struct sockaddr_in *, char *);

#endif
