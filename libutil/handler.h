#ifndef HANDLER_H
#define HANDLER_H

#include <pthread.h>

#include "buffer.h"


// endpoint is local, module or network
// external endpoints are file descriptors
enum ep_hdl_type {
    ep_src_internal,
    ep_src_external
};


/*
 * data visible inside each thread
 */
struct ep_event_view {
    struct ep_event_queue *queue;
    struct ep_event       *self;
};


/*
 * function to run as a new thread
 */
typedef void (*ep_callback_t)(int, struct ep_event_view *);


union ep_handler_id {
    int hid;
    struct ep_address *addr;
};


/*
 * memory allocated per active input?
 */
struct ep_handler {
    union ep_handler_id id;
    struct ep_buffer    buf;
    ep_callback_t       callback;
    size_t              min_input;
};


/*
 * recieves update notifications
 */
struct ep_handler_recv {
    union ep_handler_id  arr [8];
    size_t begin;
    size_t bytes;
};


struct ep_hdlset {
    struct ep_handler **hdl;
    size_t size;
    size_t avail;
};


void ep_hdlset_init(struct ep_hdlset *s, size_t max_hdl);
struct ep_handler *ep_hdlset_get(struct ep_hdlset *s, int hid);


struct ep_handler *ep_handler_create(struct ep_hdlset *s, ep_callback_t c);


#endif
