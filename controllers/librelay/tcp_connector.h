#ifndef TCP_CONNECTOR_H
#define TCP_CONNECTOR_H

int get_connection_back(int);
int send_to_relay(int, int, char *);
char *recv_bcast(int);

#endif
