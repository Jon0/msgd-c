#include <stdio.h>
#include <string.h>

#include "protocol.h"


void msg_poll_buffer(struct ep_buffer *b) {
    // recieving requests to the local server
    size_t sb = sizeof(prot_size_t);
    while (b->size > sb) {
        // the first 4 bytes contain the message length
        char *content = &b->ptr[b->begin];
        prot_size_t length = *((prot_size_t *) content);
        if (b->size >= (length + sb)) {
            msg_parse_block((content + sb), length);
            ep_buffer_release(b, (length + sb));
        }
        else {
            printf("recv length %d / %d\n", b->size, length);
            return;
        }
    }
}


void msg_push_buffer(struct ep_buffer *b, const char *msg, size_t count) {
    ep_buffer_insert(b, (char *) &count, 4);
    ep_buffer_insert(b, msg, count);
}


void msg_parse_block(const char *buf, size_t count) {
    char sbuf [256];
    memcpy(sbuf, buf, count);
    sbuf[count] = 0;
    printf("recv %d: %s\n", count, sbuf);

    struct msg_block *mb = (struct msg_block *) buf;
    switch (mb->id) {}

}
