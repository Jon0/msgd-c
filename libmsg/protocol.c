#include <stdio.h>
#include <string.h>

#include <libsys/share.h>

#include "protocol.h"


int msg_host_init(struct msg_host *h, const char *addr, const char *name) {
    h->active_id = 0;
    strcpy(h->addr, addr);
    strcpy(h->hostname, name);
    return 0;
}


size_t msg_host_read(struct msg_host *out, struct ep_buffer *in, size_t offset) {
    // one of these may segfault if buffer is too short
    ep_buffer_peek(in, out->addr, offset + 0, 32);
    ep_buffer_peek(in, out->hostname, offset + 32, 256);
    size_t treesize = ep_tree_read(&out->shared_tree, in, offset + 32 + 256);
    return 32 + 256 + treesize;
}


void msg_host_write(struct msg_host *in, struct ep_buffer *out) {
    ep_buffer_insert(out, in->addr, 32);
    ep_buffer_insert(out, in->hostname, 256);
    ep_tree_write(&in->shared_tree, out);
}


void msg_send_self(struct msg_host *h, struct ep_buffer *buf) {
    struct msg_header head;
    size_t host_count = 1;

    printf("send self\n");
    head.id = msg_type_peer_one;
    head.size = sizeof(size_t);
    head.size += 32 + 256 + ep_tree_serial_bytes(&h->shared_tree);
    printf("send %d bytes (%x)\n", head.size, buf);
    ep_buffer_insert(buf, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(buf, (char *) &host_count, sizeof(size_t));
    msg_host_write(h, buf);

    if (msg_invalid_buffer(buf)) {
        printf("error sending buffer (%d should be %d)\n", buf->size, head.size);
    }
}


void msg_host_list_debug(struct msg_host_list *h) {
    printf("\n=== host list state ===\n");
    for (int i = 0; i < h->host_count; ++i) {
        struct msg_host *host = &h->ptr[i];
        printf("[%s] Host id %d: %s\n", host->addr, i, host->hostname);
        ep_tree_print(&host->shared_tree);
        msg_tree_elems(&host->shared_tree);
    }
}

int msg_host_list_init(struct msg_host_list *h, size_t max, int file) {
    h->host_max = max;
    if (file) {
        h->ptr = ep_memfile("tree", sizeof(struct msg_host) * max);
    }
    else {
        h->ptr = malloc(sizeof(struct msg_host) * max);
    }
    h->host_count = 0;
    for (int i = 0; i < max; ++i) {
        msg_tree_init(&h->ptr[i].shared_tree);
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


void msg_merge_peers(struct msg_host_list *h, struct ep_buffer *buf, size_t offset) {
    size_t recv_hosts;
    ep_buffer_peek(buf, (char *) &recv_hosts, offset, sizeof(recv_hosts));
    offset += sizeof(recv_hosts);

    // TODO require notification of changes
    for (int i = 0; i < recv_hosts; ++i) {
        offset += msg_host_list_merge(h, buf, offset);
    }
}


size_t msg_host_list_merge(struct msg_host_list *h, struct ep_buffer *in, size_t offset) {
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
            printf("host count is now %d\n", h->host_count);
            out = &h->ptr[index];
        }
    }
    size_t treesize = ep_tree_read(&out->shared_tree, in, offset + 32 + 256);
    return 32 + 256 + treesize;
}


struct msg_host *msg_host_match(struct msg_host_list *h, const char *hostname) {
    for (int i = 0; i < h->host_count; ++i) {
        if (strcmp(hostname, h->ptr[i].hostname) == 0) {
            return &h->ptr[i];
        }
    }
    return NULL;
}


void msg_host_list_write(struct msg_host_list *h, struct ep_buffer *buf) {
    struct msg_header head;

    printf("send %d hosts\n", h->host_count);
    head.id = msg_type_peer_all;
    head.size = sizeof(size_t);
    for (int i = 0; i < h->host_count; ++i) {
        head.size += 32 + 256 + ep_tree_serial_bytes(&h->ptr[i].shared_tree);
    }
    printf("send %d bytes (%x)\n", head.size, buf);
    ep_buffer_insert(buf, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(buf, (char *) &h->host_count, sizeof(size_t));
    for (int i = 0; i < h->host_count; ++i) {
        msg_host_write(&h->ptr[i], buf);
    }

    if (msg_invalid_buffer(buf)) {
        printf("error sending buffer (%d should be %d)\n", buf->size, head.size);
    }
}


int msg_invalid_buffer(struct ep_buffer *in) {
    struct msg_message msg;
    size_t hs = sizeof(struct msg_header);
    size_t read_header;
    size_t read_body;
    if (in->size >= hs) {
        read_header = ep_buffer_peek(in, (char *) &msg.head, 0, hs);
        if (read_header == hs && in->size >= hs + msg.head.size) {
            return 0;
        }
    }
    return -1;
}


int msg_poll_message(struct ep_buffer *in, struct msg_message *out) {

    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    size_t read_header;
    size_t read_body;
    if (in->size >= hs) {
        read_header = ep_buffer_peek(in, (char *) &out->head, 0, hs);
        if (read_header == hs && in->size >= hs + out->head.size) {
            ep_buffer_release(in, hs);
            out->body = in;
            return 1;
        }
    }
    return 0;
}


void msg_write_header(struct ep_buffer *b, enum msg_type_id id, int32_t length) {
    struct msg_header head;
    head.id = id;
    head.size = length;
    ep_buffer_insert(b, (char *) &head, sizeof(head));
}


void msg_req_peer_init(struct ep_buffer *b, struct msg_host *h) {
    struct msg_header head;
    head.id = msg_type_peer_init;
    head.size = 32 + 256 + ep_tree_serial_bytes(&h->shared_tree);
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    msg_host_write(h, b);
}


void msg_req_proc_init(struct ep_buffer *b, const char *msg, size_t count) {
    struct msg_header head;
    head.id = msg_type_proc_init;
    head.size = count;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, msg, count);
}


void msg_req_avail(struct ep_buffer *b, struct ep_tree *t) {
    struct msg_header head;
    head.id = msg_type_avail;
    head.state = msg_tree_hash(t);
    head.size = 0;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
}


void msg_req_publish(struct ep_buffer *b, const char *name, size_t len, int nodeid) {
    struct msg_header head;
    head.id = msg_type_publ;
    head.size = len;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, name, len);
}


void msg_req_subscribe(struct ep_buffer *b, int nodeid, int subid) {
    struct msg_header head;
    struct msg_subscribe subs;
    head.id = msg_type_subs;
    head.size = sizeof(subs);
    subs.nodeid = nodeid;
    subs.subid = subid;
    ep_buffer_insert(b, (char *) &head, sizeof(head));
    ep_buffer_insert(b, (char *) &subs, sizeof(subs));
}


size_t msg_send_block(struct ep_buffer *buf, int node, int hdl, char *in, size_t count) {
    struct msg_header head;
    head.id = msg_type_data;
    head.size = sizeof(node) + sizeof(hdl) + count;
    ep_buffer_insert(buf, (char *) &head, sizeof(head));
    ep_buffer_insert(buf, (char *) &node, sizeof(node));
    ep_buffer_insert(buf, (char *) &hdl, sizeof(hdl));
    ep_buffer_insert(buf, in, count);
}
