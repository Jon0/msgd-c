#ifndef CMDLINE_H
#define CMDLINE_H

#include <stdlib.h>


void msg_log(const char *label, const char *msg);


void msg_read_string(char *buf, size_t count);
int msg_read_int();


#endif
