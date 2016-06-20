#include <dlfcn.h>

#include "module.h"


void open_msgd_module(struct module_private *md, const char *path) {
    md->dlm = dlopen(path, RTLD_LAZY);
    md->init = dlsym(md->dlm, "msgd_init_module");
    md->free = dlsym(md->dlm, "msgd_free_module");
    md->update = dlsym(md->dlm, "msgd_update_module");
    if (md->init) {
        md->init(&md->state);
    }
    else {
        dlerror();
    }
}


void close_msgd_module(struct module_private *md) {
    if (md->free) {
        md->free(&md->state);
    }
    int err = dlclose(md->dlm);
    if (err) {
        dlerror();
    }
}
