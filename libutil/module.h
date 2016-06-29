#ifndef MODULE_H
#define MODULE_H

#include "endpoint.h"


struct module_private {
    void  *dlm;
    void  *state;
};


void open_msgd_module(struct module_private *md, const char *path);
void close_msgd_module(struct module_private *md);

void *ep_module_func(struct module_private *md, const char *name);

#endif
