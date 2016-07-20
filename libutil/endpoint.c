#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "endpoint.h"


int ep_read_block(struct ep_source *s, size_t n) {
    char buf[1024];
    size_t copied = 0;
    while (copied < n) {
        copied += read(s->fd, &buf[copied], n - copied);
    }
    return copied;
}


void ep_table_init(struct ep_table *t) {
    size_t maxsize = 256;
    size_t addr_size = sizeof(struct ep_address) * maxsize;
    size_t src_size = sizeof(struct ep_source) * maxsize;
    size_t dest_size = sizeof(struct ep_dest) * maxsize;
    size_t total_size = addr_size + src_size + dest_size;

    // allocate and zero
    printf("alloc table (%d bytes)\n", total_size);
    char *mem = malloc(total_size);
    memset(mem, 0, total_size);
    t->addr = (struct ep_address *) mem;
    t->src = (struct ep_source *) &mem[addr_size];
    t->dest = (struct ep_dest *) &mem[addr_size + src_size];
    t->avail = maxsize;
    t->src_count = 0;
    t->next_id = 1;
}


void ep_table_free(struct ep_table *t) {
    t->avail = 0;
    t->src_count = 0;
    free(t->addr);
}


size_t ep_table_hash(struct ep_table *t, int epid) {
    return epid % t->avail;
}


struct ep_handler *ep_new_hdl(struct ep_table *t, ep_callback_t c) {
    int id = t->next_id++;
    size_t array_pos = ep_table_hash(t, id);
    for (size_t i = 0; i < t->avail; ++i) {
        struct ep_handler *item = &t->hdl[(array_pos + i) % t->avail];
        if (item->epid == 0) {

            // allocate memory for buffer and handler
            size_t buffer_size = 4096;
            ep_buffer_init(&item->buf, malloc(buffer_size), buffer_size);
            item->epid = id;
            item->callback = c;
            item->min_input = 0;
            return item;
        }
    }
    printf("array is full\n");
    return NULL;
}


struct ep_address *ep_new_addr(struct ep_table *t, int epid) {
    size_t array_pos = ep_table_hash(t, epid);
    for (size_t i = 0; i < t->avail; ++i) {
        struct ep_address *item = &t->addr[(array_pos + i) % t->avail];
        if (item->epid == 0) {
            item->epid = epid;
            item->addrlen = 0;
            item->src = NULL;
            item->dest = NULL;
            return item;
        }
    }
    printf("array is full\n");
    return NULL;
}


struct ep_source *ep_new_src(struct ep_table *t, int epid) {
    size_t array_pos = ep_table_hash(t, epid);

    // ensure there is an address entry
    struct ep_address *addr = ep_table_addr(t, epid);
    if (addr) {
        for (size_t i = 0; i < t->avail; ++i) {
            struct ep_source *item = &t->src[(array_pos + i) % t->avail];
            if (item->epid == 0) {
                addr->src = item;
                item->epid = epid;
                return item;
            }
        }
    }
    printf("array is full\n");
    return NULL;
}


struct ep_dest *ep_new_dest(struct ep_table *t, int epid) {
    size_t array_pos = ep_table_hash(t, epid);

    // ensure there is an address entry
    struct ep_address *addr = ep_table_addr(t, epid);
    if (addr) {
        for (size_t i = 0; i < t->avail; ++i) {
            struct ep_dest *item = &t->dest[(array_pos + i) % t->avail];
            if (item->epid == 0) {
                addr->dest = item;
                item->epid = epid;
                return item;
            }
        }
    }
    printf("array is full\n");
    return NULL;
}


struct ep_address *ep_table_addr(struct ep_table *t, int epid) {
    size_t array_pos = ep_table_hash(t, epid);
    for (size_t i = 0; i < t->avail; ++i) {
        struct ep_address *item = &t->addr[(array_pos + i) % t->avail];
        if (item->epid == epid) {
            return item;
        }
    }
    printf("epid not found: %d\n", epid);
    return NULL;
}


struct ep_source *ep_table_src(struct ep_table *t, int epid) {
    size_t array_pos = ep_table_hash(t, epid);
    for (size_t i = 0; i < t->avail; ++i) {
        struct ep_source *item = &t->src[(array_pos + i) % t->avail];
        if (item->epid == epid) {
            return item;
        }
    }
    printf("epid not found: %d\n", epid);
    return NULL;
}


struct ep_dest *ep_table_dest(struct ep_table *t, int epid) {
    size_t array_pos = ep_table_hash(t, epid);
    for (size_t i = 0; i < t->avail; ++i) {
        struct ep_dest *item = &t->dest[(array_pos + i) % t->avail];
        if (item->epid == epid) {
            return item;
        }
    }
    printf("epid not found: %d\n", epid);
    return NULL;
}


struct ep_handler *ep_table_hdl(struct ep_table *t, int epid) {
    size_t array_pos = ep_table_hash(t, epid);
    for (size_t i = 0; i < t->avail; ++i) {
        struct ep_handler *item = &t->hdl[(array_pos + i) % t->avail];
        if (item->epid == epid) {
            return item;
        }
    }
    printf("epid not found: %d\n", epid);
    return NULL;
}
