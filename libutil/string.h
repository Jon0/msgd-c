#ifndef LIBUTIL_STRING_H
#define LIBUTIL_STRING_H

#include "buffer.h"


struct msgu_string {
    union {
        const char *cstr;
        int         strid;
    } data;
};


void ep_string_read(struct msgu_buffer *b, char *out, size_t count);
void ep_string_write(struct msgu_buffer *b, char *in, size_t count);


#endif
