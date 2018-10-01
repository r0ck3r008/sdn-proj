#ifndef LINK_H
#define LINK_H

void add_node(struct controller *, char *, int);
void del_node(int);
struct bcast_msg_node *iterate(int);
void equate_controllers(struct controller *, struct controller *);

#endif
