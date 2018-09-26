#ifndef SERVER_H
#define SERVER_H

int create_sock(char *);
int server_workings(char *);
int connect_back(struct controller *);
int publish(char *);
void *cli_run(void *);
void *alarm_run(void *);

#endif
