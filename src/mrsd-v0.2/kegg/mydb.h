#include <stdio.h>
#define KL 16 
#ifndef uchar
#define uchar unsigned char
#endif
#define GDBM_FILE int
#define DL 12
GDBM_FILE my_connect(GDBM_FILE ms,char *name);
int my_fetch(GDBM_FILE ms,uchar *key,uchar *data,int *length);
int my_store(GDBM_FILE ms,uchar *key,uchar *data,int length);
void my_close(GDBM_FILE ms);
void my_sync(GDBM_FILE ms);
