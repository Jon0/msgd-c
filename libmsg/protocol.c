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
    head.type = id;
    head.size = length;
    ep_buffer_insert(b, (char *) &head, sizeof(head));
}


void msg_req_share(struct ep_buffer *b, const char *path) {
    struct msg_header head;
    head.type = msg_type_share_file;
    head.size = strlen(path);
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, path, head.size);
}


void msg_req_peer_init(struct ep_buffer *b, struct msg_host *h) {
    struct msg_header head;
    head.type = msg_type_peer_init;
    head.size = 32 + 256 + ep_share_set_size(&h->shares);
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    msg_host_write(h, b);
}


void msg_req_proc_init(struct ep_buffer *b, const char *msg, size_t count) {
    struct msg_header head;
    head.type = msg_type_share_proc;
    head.size = count;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, msg, count);
}


size_t msg_send_block(struct ep_buffer *buf, int node, int hdl, char *in, size_t count) {
    struct msg_header head;
    head.type = msg_type_data;
    head.size = sizeof(node) + sizeof(hdl) + count;
    ep_buffer_insert(buf, (char *) &head, sizeof(head));
    ep_buffer_insert(buf, (char *) &node, sizeof(node));
    ep_buffer_insert(buf, (char *) &hdl, sizeof(hdl));
    ep_buffer_insert(buf, in, count);
}


void msg_send_host(struct msg_host *h, struct ep_buffer *buf) {
    struct msg_header head;
    size_t host_count = 1;

    printf("send self\n");
    head.type = msg_type_peer_one;
    head.size = sizeof(size_t);
    head.size += 32 + 256 + ep_share_set_size(&h->shares);
    printf("send %d bytes\n", head.size);
    ep_buffer_insert(buf, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(buf, (char *) &host_count, sizeof(size_t));
    msg_host_write(h, buf);

    if (msg_invalid_buffer(buf)) {
        printf("error sending buffer (%lu should be %d)\n", buf->size, head.size);
    }
}


void msg_send_host_list(struct msg_host_list *h, struct ep_buffer *buf) {
    struct msg_header head;

    printf("send %lu hosts\n", h->host_count);
    head.type = msg_type_peer_all;
    head.size = sizeof(size_t);
    for (int i = 0; i < h->host_count; ++i) {
        head.size += 32 + 256 + ep_share_set_size(&h->ptr[i].shares);
    }
    printf("send %d bytes\n", head.size);
    ep_buffer_insert(buf, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(buf, (char *) &h->host_count, sizeof(size_t));
    for (int i = 0; i < h->host_count; ++i) {
        msg_host_write(&h->ptr[i], buf);
    }

    if (msg_invalid_buffer(buf)) {
        printf("error sending buffer (%lu should be %d)\n", buf->size, head.size);
    }
}
