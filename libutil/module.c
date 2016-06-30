#include <dlfcn.h>

#include "module.h"


void ep_module_open(struct ep_module *md, const char *path) {
    md->dlm = dlopen(path, RTLD_LAZY);
}


void ep_module_close(struct ep_module *md) {
    int err = dlclose(md->dlm);
    if (err) {
        dlerror();
    }
}


void *ep_module_func(struct ep_module *md, const char *name) {
    void *fn = dlsym(md->dlm, "msgd_init_module");
    if (!fn) {
        dlerror();
    }
    return fn;
}
