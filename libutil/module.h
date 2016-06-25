#ifndef MODULE_H
#define MODULE_H

#include <libmsg/node.h>


struct module_state {
    char name [256];
    int mode;
    void *ptr;
};


/*
 * these specific function types can be moved into msgd
 */
typedef void (*init_module)(struct module_state *ms, struct node_buffer *nb);
typedef void (*free_module)(struct module_state *ms);


struct module_private {
    struct module_state   state;
    void                 *dlm;
    init_module           init;
    free_module           free;
};


void open_msgd_module(struct module_private *md, struct node_buffer *nb, const char *path);
void close_msgd_module(struct module_private *md);

#endif
