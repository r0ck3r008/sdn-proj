#ifndef LINKS_H
#define LINKS_H

void add_node(union list *, union list *, int);
void equate(union list *, union list *, int);
int del_node(union list *, char *, int, int);
union list *iterate(union list *, char *, int, int);
int list_len(union list *);

#endif
