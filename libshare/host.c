#include <stdio.h>
#include <string.h>

#include "host.h"


int msg_host_init(struct msg_host *h, const char *addr, const char *name) {
    h->active_id = 0;
    strcpy(h->addr, addr);
    strcpy(h->hostname, name);
    return 0;
}


size_t msg_host_read(struct msg_host *out, struct msgu_stream *s) {
    // one of these may segfault if buffer is too short
    msgu_stream_read(s, out->addr, 32);
    msgu_stream_read(s, out->hostname, 256);
    size_t size = ep_share_set_read(&out->shares, s);
    return 32 + 256 + size;
}


void msg_host_write(struct msg_host *in, struct msgu_stream *s) {
    msgu_stream_write(s, in->addr, 32);
    msgu_stream_write(s, in->hostname, 256);
    ep_share_set_write(&in->shares, s);
}


void msg_host_list_debug(struct msg_host_list *h) {
    printf("\n=== host list state ===\n");
    for (int i = 0; i < h->host_count; ++i) {
        struct msg_host *host = &h->ptr[i];
        printf("[%s] Host id %d: %s\n", host->addr, i, host->hostname);
        ep_share_set_print(&host->shares);
    }
}


int msg_host_list_init(struct msg_host_list *h) {
    h->host_count = 0;
    h->host_max = 0;
    h->ptr = NULL;
}


int msg_host_list_alloc(struct msg_host_list *h, size_t max) {
    h->ptr = malloc(sizeof(struct msg_host) * max);
    for (int i = 0; i < max; ++i) {
        ep_share_set_init(&h->ptr[i].shares);
    }
    h->host_max = max;
}


int msg_host_list_add(struct msg_host_list *h, const char *addr, const char *name) {
    if (h->host_count == h->host_max) {
        return -1;
    }
    int index = h->host_count++;
    msg_host_init(&h->ptr[index], addr, name);
    return index;
}


void msg_merge_peers(struct msg_host_list *h, struct msgu_stream *s) {
    size_t recv_hosts;
    msgu_stream_read(s, (char *) &recv_hosts, sizeof(recv_hosts));

    // TODO require notification of changes
    for (int i = 0; i < recv_hosts; ++i) {
        msg_host_list_merge(h, s);
    }
}


size_t msg_host_list_merge(struct msg_host_list *h, struct msgu_stream *s) {
    char addr [32];
    char hostname [256];
    msgu_stream_read(s, addr, 32);
    msgu_stream_read(s, hostname, 256);
    printf("recv host %s, %s\n", addr, hostname);

    // try match existing hosts
    struct msg_host *out = msg_host_match(h, hostname);
    if (!out) {
        printf("creating new entry\n");
        int index = msg_host_list_add(h, addr, hostname);
        if (index < 0) {
            return 0;
        }
        else {
            printf("host count is now %lu\n", h->host_count);
            out = &h->ptr[index];
        }
    }
    size_t size = ep_share_set_read(&out->shares, s);
    return 32 + 256 + size;
}


struct msg_host *msg_host_match(struct msg_host_list *h, const char *hostname) {
    for (int i = 0; i < h->host_count; ++i) {
        if (strcmp(hostname, h->ptr[i].hostname) == 0) {
            return &h->ptr[i];
        }
    }
    return NULL;
}
