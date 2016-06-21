#ifndef MODULE_H
#define MODULE_H


struct module_state {
    char name [256];
    int mode;
    void *ptr;
};


typedef void (*init_module)(struct module_state *ms);
typedef void (*free_module)(struct module_state *ms);
typedef void (*update_module)(struct module_state *ms);


struct module_private {
    struct module_state   state;
    void                 *dlm;
    init_module           init;
    free_module           free;
    update_module         update;
};


void open_msgd_module(struct module_private *md, const char *path);
void close_msgd_module(struct module_private *md);

#endif
