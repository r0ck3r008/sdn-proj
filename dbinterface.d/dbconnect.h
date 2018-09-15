#ifndef DBCONNECT_H
#define DBCONNECT_H
#define NEEDS_ALL

#include"global_defs.h"
char *get_passwd();
int dbconnect(char *);
int query_db(struct client *, char *, int);

#endif
