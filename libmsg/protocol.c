#include <stdio.h>
#include <string.h>

#include "protocol.h"


void msg_poll_buffer(struct msg_tree *tree, struct ep_buffer *b) {
    struct msg_header head;

    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    while (b->size > hs) {
        size_t s = ep_buffer_erase(b, (char *) &head, hs);
        msg_parse_block(tree, &head);
    }
}


void msg_push_buffer(struct ep_buffer *b, const char *msg, size_t count) {
    ep_buffer_insert(b, (char *) &count, 4);
    ep_buffer_insert(b, msg, count);
}


void msg_parse_block(struct msg_tree *tree, struct msg_header *h) {
    printf("recv %d\n", h->id);


}
