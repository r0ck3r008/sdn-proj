#ifndef LIST_H
#define LIST_H

void add_node(union node *, union node *, int);
void _general_equate(union node *, union node *, int);
void _general_equate_start(union node *, union node *);
void _equate_ctrlr(union node *, union node *);
void _equate_bmn(union node *, union node *);
union node *find_node(union node *, int);
int del_node(union node *, int, int);
int list_len(union node *);

#endif
