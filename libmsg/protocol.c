#include <stdio.h>
#include <string.h>

#include "protocol.h"


void msg_poll_buffer(struct msg_tree *tree, struct ep_buffer *b) {
    struct msg_header head;
    char body [256];

    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    size_t s;
    while (b->size >= hs) {
        s = ep_buffer_erase(b, (char *) &head, hs);
        s = ep_buffer_erase(b, body, head.size);
        msg_parse_block(tree, &head, body, s);
    }
}


void msg_parse_block(struct msg_tree *tree, struct msg_header *h, char *data, size_t count) {
    printf("recv %d\n", h->id);

    msg_tree_add_proc(tree, data, count);
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
