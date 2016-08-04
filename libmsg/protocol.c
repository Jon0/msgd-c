#include <stdio.h>
#include <string.h>

#include "protocol.h"


void msg_poll_apply(struct msg_tree *tree, struct msg_request *r) {
    struct msg_message m;
    ep_sink_print(r->src);


    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    size_t s;
    while (r->buf->size >= hs) {
        s = ep_buffer_erase(r->buf, (char *) &m.head, hs);
        s = ep_buffer_erase(r->buf, m.body, m.head.size);
        msg_parse(tree, &m, r->src);
    }
}


void msg_parse(struct msg_tree *tree, struct msg_message *m, struct ep_sink *out) {
    printf("recv type %d (%d)\n", m->head.id, m->head.size);
    switch (m->head.id) {
    case msg_type_proc:
        msg_tree_add_proc(tree, m->body, m->head.size);
        break;
    case msg_type_avail:
        msg_rsp_avail(tree, out);
        break;
    }
}


void msg_req_addproc(struct ep_buffer *b, const char *msg, size_t count) {
    struct msg_header head;
    head.id = msg_type_proc;
    head.size = count;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, msg, count);
}


void msg_req_avail(struct ep_buffer *b) {
    struct msg_header head;
    head.id = msg_type_avail;
    head.size = 0;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
}


size_t msg_rsp_avail(struct msg_tree *tree, struct ep_sink *s) {
    struct msg_header head;
    head.id = msg_type_avail;
    head.size = 0;

    printf("sending reply (%d)\n", s->epid);
    ep_write_blk(s, (char *) &head, sizeof(struct msg_header));
}
