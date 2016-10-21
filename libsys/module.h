#ifndef MODULE_H
#define MODULE_H

#include "endpoint.h"


struct ep_module {
    void  *dlm;
};


void ep_module_open(struct ep_module *md, const char *path);
void ep_module_close(struct ep_module *md);

void *ep_module_func(struct ep_module *md, const char *name);

#endif
