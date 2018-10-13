#ifndef SND_RCV_H
#define SND_RCV_H

int snd(int, char *, char *, char *);
char *rcv(int, char *, char *);
int snd_to(int, struct sockaddr_in *, char *, char *, char *);

#endif
