#include <stdio.h>
#include <string.h>

#include <libsys/memory.h>

#include "host.h"


int msg_host_init(struct msg_host *h, const char *addr, const char *name) {
    h->active_id = 0;
    strcpy(h->addr, addr);
    strcpy(h->hostname, name);
    return 0;
}


size_t msg_host_read(struct msg_host *out, struct msgu_buffer *in, size_t offset) {
    // one of these may segfault if buffer is too short
    ep_buffer_peek(in, out->addr, offset + 0, 32);
    ep_buffer_peek(in, out->hostname, offset + 32, 256);
    size_t size = ep_share_set_read(&out->shares, in, offset + 32 + 256);
    return 32 + 256 + size;
}


void msg_host_write(struct msg_host *in, struct msgu_buffer *out) {
    ep_buffer_insert(out, in->addr, 32);
    ep_buffer_insert(out, in->hostname, 256);
    ep_share_set_write(&in->shares, out);
}


void msg_host_list_debug(struct msg_host_list *h) {
    printf("\n=== host list state ===\n");
    for (int i = 0; i < h->host_count; ++i) {
        struct msg_host *host = &h->ptr[i];
        printf("[%s] Host id %d: %s\n", host->addr, i, host->hostname);
        ep_share_set_print(&host->shares);
    }
}


int msg_host_list_init(struct msg_host_list *h, size_t max, int file) {
    h->host_max = max;
    if (file) {
        h->ptr = msgs_memfile("hostlist", sizeof(struct msg_host) * max);
    }
    else {
        h->ptr = malloc(sizeof(struct msg_host) * max);
    }
    h->host_count = 0;
    for (int i = 0; i < max; ++i) {
        ep_share_set_init(&h->ptr[i].shares);
    }
}


int msg_host_list_add(struct msg_host_list *h, const char *addr, const char *name) {
    if (h->host_count == h->host_max) {
        return -1;
    }
    int index = h->host_count++;
    msg_host_init(&h->ptr[index], addr, name);
    return index;
}


void msg_merge_peers(struct msg_host_list *h, struct msgu_buffer *buf, size_t offset) {
    size_t recv_hosts;
    ep_buffer_peek(buf, (char *) &recv_hosts, offset, sizeof(recv_hosts));
    offset += sizeof(recv_hosts);

    // TODO require notification of changes
    for (int i = 0; i < recv_hosts; ++i) {
        offset += msg_host_list_merge(h, buf, offset);
    }
}


size_t msg_host_list_merge(struct msg_host_list *h, struct msgu_buffer *in, size_t offset) {
    char addr [32];
    char hostname [256];
    ep_buffer_peek(in, addr, offset + 0, 32);
    ep_buffer_peek(in, hostname, offset + 32, 256);
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
    size_t size = ep_share_set_read(&out->shares, in, offset + 32 + 256);
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
