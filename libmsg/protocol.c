#include <stdio.h>
#include <string.h>

#include "protocol.h"


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
    msg_write_host(h, b);
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
    head.id = msg_type_subs;
    head.size = sizeof(int) * 2;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, (char *) &nodeid, sizeof(int));
    ep_buffer_insert(b, (char *) &subid, sizeof(int));
}


void msg_send_peers(struct ep_buffer *buf, struct msg_host *h, size_t host_count) {
    printf("send %d hosts\n", host_count);
    struct msg_header head;
    head.id = msg_type_peer_all;
    head.size = sizeof(size_t);
    for (int i = 0; i < host_count; ++i) {
        head.size += 32 + 256 + ep_tree_serial_bytes(&h[i].shared_tree);
    }
    printf("send %d bytes (%x)\n", head.size, buf);
    ep_buffer_insert(buf, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(buf, (char *) &host_count, sizeof(size_t));
    for (int i = 0; i < host_count; ++i) {
        printf("send host %d\n", i);
        msg_write_host(&h[i], buf);
    }

    if (msg_invalid_buffer(buf)) {
        printf("error sending buffer (%d should be %d)\n", buf->size, head.size);
    }
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


void msg_merge_peers(struct ep_buffer *buf, struct msg_host *h, size_t *host_count, size_t host_limit) {
    size_t recv_hosts;
    ep_buffer_peek(buf, (char *) &recv_hosts, 0, sizeof(recv_hosts));
    size_t offset = sizeof(recv_hosts);

    // TODO require notification of changes
    for (int i = 0; i < recv_hosts; ++i) {
        offset += msg_host_merge(buf, offset, h, host_count);
    }
}


void msg_write_host(struct msg_host *in, struct ep_buffer *out) {
    ep_buffer_insert(out, in->addr, 32);
    ep_buffer_insert(out, in->hostname, 256);
    msg_tree_send(&in->shared_tree, out);
}


size_t msg_host_recv(struct ep_buffer *in, struct msg_host *out, size_t offset) {
    // one of these may segfault if buffer is too short
    ep_buffer_peek(in, out->addr, offset + 0, 32);
    ep_buffer_peek(in, out->hostname, offset + 32, 256);
    size_t treesize = ep_tree_read(&out->shared_tree, in, offset + 32 + 256);
    return 32 + 256 + treesize;
}


size_t msg_host_merge(struct ep_buffer *in, size_t offset, struct msg_host *h, size_t *host_count) {
    char addr [32];
    char hostname [256];
    ep_buffer_peek(in, addr, offset + 0, 32);
    ep_buffer_peek(in, hostname, offset + 32, 256);
    printf("recv host %s, %s\n", addr, hostname);

    // try match existing hosts
    struct msg_host *out = msg_host_match(h, *host_count, hostname);
    if (!out) {
        printf("creating new entry\n");
        out = &h[(*host_count)++];
        strcpy(out->addr, addr);
        strcpy(out->hostname, hostname);
        printf("host count is now %d\n", *host_count);
    }
    size_t treesize = ep_tree_read(&out->shared_tree, in, offset + 32 + 256);
    return 32 + 256 + treesize;
}


struct msg_host *msg_host_match(struct msg_host *h, size_t host_count, const char *hostname) {
    for (int i = 0; i < host_count; ++i) {
        if (strcmp(hostname, h[i].hostname) == 0) {
            return &h[i];
        }
    }
    return NULL;
}


void msg_tree_send(struct ep_tree *tree, struct ep_buffer *out) {
    ep_tree_write(tree, out);
}
