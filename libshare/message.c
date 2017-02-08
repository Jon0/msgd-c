#include <stdio.h>
#include <string.h>

#include "message.h"


void msgu_message_free(struct msgu_message *m) {
    msgu_msgdata_free(&m->buf);
}


void msgu_message_print(char *buf, const struct msgu_message *m) {
    char data_buf [100];
    msgu_msgdata_print(data_buf, &m->buf);
    const char *fmt = "event: %s, id: %d, size: %d, hash: %x, data: %s";
    sprintf(buf, fmt, msgu_msgtype_str(m->event_type), m->event_id, m->data_size, m->data_hash, data_buf);
}


size_t msgu_message_size_frag(const void *m) {
    const struct msgu_message *msg = m;
    return sizeof(msg->event_type) +
           sizeof(msg->event_id) +
           sizeof(msg->data_size) +
           sizeof(msg->data_hash) +
           msgu_msgdata_size_frag(&msg->buf);
}


int msgu_message_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *m) {
    struct msgu_message *msg = m;
    static msgu_obj_read_t msgu_message_read_fns[] = {
        msgu_i32_read_frag,
        msgu_i32_read_frag,
        msgu_i32_read_frag,
        msgu_i32_read_frag,
        msgu_msgdata_read_frag,
    };
    void *layout[] = {
        &msg->event_type,
        &msg->event_id,
        &msg->data_size,
        &msg->data_hash,
        &msg->buf,
    };
    return msgu_read_many(src, f, msgu_message_read_fns, layout, 5);
}


int msgu_message_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *m) {
    const struct msgu_message *msg = m;
    static msgu_obj_write_t msgu_message_write_fns[] = {
        msgu_i32_write_frag,
        msgu_i32_write_frag,
        msgu_i32_write_frag,
        msgu_i32_write_frag,
        msgu_msgdata_write_frag,
    };
    const void *layout[] = {
        &msg->event_type,
        &msg->event_id,
        &msg->data_size,
        &msg->data_hash,
        &msg->buf,
    };
    return msgu_write_many(dest, f, msgu_message_write_fns, layout, 5);
}


hash_t msgu_message_hash(const void *m) {
    return 0;
}


int msgu_message_cmp(const void *a, const void *b) {
    return 0;
}
