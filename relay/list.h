#ifndef LIST_H
#define LIST_H

void add_node(union node *, union node *, int);
void general_equate(union node *, union node *, int);
union node *find_node(union node *, int);
int del_node(union node *, int, int);
int list_len(union node *);

#endif
