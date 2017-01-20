#ifndef LIBUTIL_HOST_H
#define LIBUTIL_HOST_H

#include "buffer.h"
#include "map.h"
#include "share.h"


/*
 * per remote host information
 * includes list of all available shares
 */
struct msg_host {
    int active_id;
    char addr [32];
    char hostname [256];

    // tree containing struct msg_node
    struct msgu_share_set shares;
};


struct msg_host_list {
    struct msg_host *ptr;
    size_t host_count;
    size_t host_max;
};


int msg_host_init(struct msg_host *h, const char *addr, const char *name);
size_t msg_host_read(struct msg_host *out, struct msgu_stream *s);
void msg_host_write(struct msg_host *in, struct msgu_stream *s);


void msg_host_list_debug(struct msg_host_list *h);
int msg_host_list_init(struct msg_host_list *h);
int msg_host_list_alloc(struct msg_host_list *h, size_t max);
int msg_host_list_add(struct msg_host_list *h, const char *addr, const char *name);
void msg_merge_peers(struct msg_host_list *h, struct msgu_stream *s);
size_t msg_host_list_merge(struct msg_host_list *h, struct msgu_stream *s);
struct msg_host *msg_host_match(struct msg_host_list *h, const char *hostname);



/*
 * send and recieve hosts
 */
void msg_send_host(struct msg_host *h, struct msgu_stream *s);
void msg_send_host_list(struct msg_host_list *h, struct msgu_stream *s);

#endif
