#include <dlfcn.h>

#include "module.h"


void open_msgd_module(struct module_private *md, const char *path) {
    md->dlm = dlopen(path, RTLD_LAZY);
}


void close_msgd_module(struct module_private *md) {
    int err = dlclose(md->dlm);
    if (err) {
        dlerror();
    }
}


void *ep_module_func(struct module_private *md, const char *name) {
    void *fn = dlsym(md->dlm, "msgd_init_module");
    if (!fn) {
        dlerror();
    }
    return fn;
}
