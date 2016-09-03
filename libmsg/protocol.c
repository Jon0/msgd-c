#include <stdio.h>
#include <string.h>

#include "protocol.h"


void msg_poll_apply(struct ep_tree *tree, struct msg_request *r) {
    struct msg_message m;
    printf("recv message from ");
    ep_sink_print(r->src);


    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    size_t read_header;
    size_t read_body;
    while (r->buf->size >= hs) {
        read_header = ep_buffer_peek(r->buf, (char *) &m.head, hs);
        if (read_header == hs) {
            read_body = ep_buffer_peek(r->buf, m.body, m.head.size);
            if (read_body == m.head.size) {
                ep_buffer_release(r->buf, hs + m.head.size);
                msg_parse(tree, &m, r->src);
            }
        }
    }
}


void msg_parse(struct ep_tree *tree, struct msg_message *m, struct ep_sink *out) {
    printf("recv type %d (%d)\n", m->head.id, m->head.size);
    switch (m->head.id) {
    case msg_type_proc:
        msg_tree_add_proc(tree, m->body, m->head.size);
        msg_tree_send(tree, out);
        break;
    case msg_type_avail:
        msg_tree_send(tree, out);
        break;
    }
}


int msg_read(struct ep_table *t, int epid, struct msg_message *out) {
    // client reads replies
    int r = ep_table_read(t, epid, (char *) &out->head, sizeof(struct msg_header));
    if (r == sizeof(struct msg_header)) {
        printf("recv reply (%d, %d)\n", out->head.id, out->head.size);
        r = ep_table_read(t, epid, out->body, out->head.size);
        return 1;
    }
    return 0;
}


void msg_req_addproc(struct ep_buffer *b, const char *msg, size_t count) {
    struct msg_header head;
    head.id = msg_type_proc;
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


void msg_req_publish(struct ep_buffer *b, const char *name, size_t len) {
    struct msg_header head;
    head.id = msg_type_publ;
    head.size = len;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, name, len);
}


void msg_tree_send(struct ep_tree *tree, struct ep_sink *out) {
    ep_tree_send(tree, out);
}
