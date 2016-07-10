#ifndef HANDLER_H
#define HANDLER_H

#include "buffer.h"


// endpoint is local, module or network
// external endpoints are file descriptors
enum ep_type {
    ep_src_internal,
    ep_src_external
};



/*
 * a thread to handle events
 * the src type determines how the thread waits for input
 */
struct ep_handler {
    int              hdlid;
    int              state;
    pthread_t        thread;
    enum ep_type     src_type;
    struct ep_src   *src;
    struct ep_buffer buf;
};


/*
 * collection of handler threads
 */
struct ep_handler_pool {

};


/*
 * write input to a handler thread
 */
void ep_handler_write(struct ep_handler *h, struct ep_buffer *b);


#endif
