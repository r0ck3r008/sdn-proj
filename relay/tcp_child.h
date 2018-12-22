#ifndef TCP_CHILD_H
#define TCP_CHILD_H

int tcp_child();
void *_cli_run(void *);
union node *_client_helper(int);
int _connect_back(struct controller *);
union node *_allocate_new(int);

#endif
