#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <stdlib.h>
#include <pthread.h>
#include <netinet/in.h>

#include <libutil/buffer.h>
#include <libutil/map.h>


struct ep_table;


/*
 * either a buffer or file descriptor
 * serialising types need to be functions
 */
struct ep_stream {
    int mode;
    union {
        int fd;
        struct ep_buffer buf;
    } data;
};


/*
 * data visible inside each thread
 */
struct ep_event_view {
    int                 src;
    int                 epid;
    struct ep_handler  *self;
};


/*
 * function to run as a new thread
 */
typedef void (*ep_callback_t)(int, struct ep_event_view *);

/*
 * create and return a handler for accepted sockets
 * this may need to return many handlers
 * the output can return max of 32 ints
 */
typedef void (*ep_accept_t)(struct ep_table *t, int epid, void *in);


/*
 * each epid is either an address, or a handler
 * memory allocated per active input?
 */
struct ep_handler {
    struct ep_buffer    buf;
    pthread_mutex_t     mutex;
    ep_callback_t       callback;
    size_t              min_input;
    void               *data;
};


/*
 * give address for a remote
 */
struct ep_address {
    char               data [32];
    socklen_t          len;
};


struct ep_acceptor {
    struct ep_address addr;
    int               fd;
    ep_accept_t       on_accept;
    void             *data;
};


/*
 * moves input from file descriptors into handlers
 */
struct ep_channel {
    struct ep_buffer  write_buf;
    struct ep_address addr;
    int               fd;
};


/*
 * memory shared between processes
 */
struct ep_memory {
    int               fd;
    void             *data;
};


/*
 * monitor changes to an existing file system
 */
struct ep_notify {
    int wd;
    ep_callback_t callback;
};

int ep_notify_create();
int ep_notify_read(int infd);

void ep_handler_init(struct ep_handler *h, size_t size, ep_callback_t c, void *d);
void ep_notify_init(struct ep_notify *n, int infd, const char *path);


size_t ep_channel_flush(struct ep_channel *c);

void ep_address_print(struct ep_address *a);

#endif
