#include <stdio.h>
#include <string.h>

#include "cmdline.h"


void msg_read_string(char *buf, size_t count) {
    fgets(buf, count, stdin);
    buf[strlen(buf) - 1] = 0;
}


int msg_read_int() {
    char *end;
    char inbuf [32];
    msg_read_string(inbuf, sizeof(inbuf));
    return strtol(inbuf, &end, 10);
}
