#ifndef TCP_CHILD_H
#define TCP_CHILD_H

void tcp_child();
void *cli_run(void *);
int connect_back(struct controller *);

#endif
