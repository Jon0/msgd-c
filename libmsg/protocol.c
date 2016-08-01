#include <stdio.h>
#include <string.h>

#include "protocol.h"


void msg_poll_apply(struct msg_tree *tree, struct msg_request *r) {
    struct ep_address addr;
    struct msg_header head;
    char body [256];

    // print addr
    ep_table_addr(r->queue->table, r->srcid, &addr);
    ep_address_print(&addr);


    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    size_t s;
    while (r->msgbuf->size >= hs) {
        s = ep_buffer_erase(r->msgbuf, (char *) &head, hs);
        s = ep_buffer_erase(r->msgbuf, body, head.size);
        s = msg_parse_block(tree, &head, body, s);

        // if the message has a reply
        if (s > 0) {
            ep_queue_wblk(r->queue, r->srcid, 0, (char *) &head, hs);
        }
    }
}


size_t msg_parse_block(struct msg_tree *tree, struct msg_header *h, char *data, size_t count) {
    printf("recv %d\n", h->id);
    switch (h->id) {
    case msg_type_proc:
        msg_tree_add_proc(tree, data, count);
        return 0;
    case msg_type_avail:
        msg_rsp_avail(tree, h, data);
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


void msg_req_avail(struct ep_buffer *b) {
    struct msg_header head;
    head.id = msg_type_avail;
    head.size = 0;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
}


void msg_rsp_avail(struct msg_tree *tree, struct msg_header *h, char *data) {

}
