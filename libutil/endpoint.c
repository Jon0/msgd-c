#include "endpoint.h"


void ep_trigger(struct ep_handler_map *map, int fd) {
    // lookup the internal endpoint id
    // listeners require special handling
    // otherwise copy into a ring buffer
    for (int i = 0; i < map->handler_count; ++i) {
        if (map->handlers[0].fd == fd) {
            map->handlers[0].fn();
        }
    }


    // share the ring buffer
}


void ep_init(struct ep_table *t, char *path) {

}


int ep_open(struct ep_table *t, int type) {

}


void ep_poll(struct ep_table *t, struct thread_pool *p) {
    // lock mutex
    // wait for any endpoint with input
    int r = poll(t->fds, t->fdcount, -1);

    // find which fds recv input or error
    for (int i = 0; i < t->fdcount; ++i) {
        r = poll(&t->fds[i], 1, 0);
        if (r) {
            ep_trigger(&t->fdmap, t->fds[i].fd);
        }
    }


    // wait for (only reading) thread tasks to complete?
    // apply modifications to fd listeners
    // including closing this fd

    // restart loop (write tasks can continue working)
}


void ep_modify(struct ep_table *t, struct ep_table_delta *dt) {

}
