#ifndef LIBUTIL_STRING_H
#define LIBUTIL_STRING_H


struct msgu_string {
    union {
        const char *cstr;
        int         strid;
    } data;
};


#endif
