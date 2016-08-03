#include <stdio.h>
#include <string.h>

#include "protocol.h"


void msg_poll_apply(struct msg_tree *tree, struct msg_request *r) {
    struct ep_address addr;
    struct msg_message m;

    // print addr
    ep_table_addr(r->queue->table, r->ev->srcid, &addr);
    ep_address_print(&addr);


    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    size_t s;
    while (r->msgbuf->size >= hs) {
        s = ep_buffer_erase(r->msgbuf, (char *) &st.head, hs);
        s = ep_buffer_erase(r->msgbuf, st.body, st.head.size);
        s = msg_parse_block(tree, &m, buf);

        // if the message has a reply
        if (s > 0) {
            printf("sending reply (%d)\n", s);
            ep_queue_reply(r->queue, r->ev, (char *) &st.head, hs);
        }
    }
}


void msg_parse(struct msg_tree *tree, struct msg_message *m, struct ep_sink *out) {
    printf("recv type %d (%d)\n", m->head.id, count);
    switch (m->head.id) {
    case msg_type_proc:
        msg_tree_add_proc(tree, m->body, m->bsize);
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


size_t msg_rsp_avail(struct msg_tree *tree, struct ep_buffer *b) {
    struct msg_header head;
    head.id = msg_type_avail;
    head.size = 0;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
}
