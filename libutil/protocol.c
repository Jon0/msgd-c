#include <stdio.h>
#include <string.h>

#include "protocol.h"


int msg_invalid_buffer(struct msgu_buffer *in) {
    struct msgu_header msg;
    size_t hs = sizeof(struct msgu_header);
    size_t read_header;
    size_t read_body;
    if (in->size >= hs) {
        read_header = ep_buffer_peek(in, (char *) &msg, 0, hs);
        if (read_header == hs && in->size >= hs + msg.size) {
            return 0;
        }
    }
    return -1;
}


int msg_poll_message(struct msgu_stream *in, struct msgu_read_status *out) {

    // recieving requests to the local server
    char *buf = (char *) &out->header;
    size_t headsize = sizeof(struct msgu_header);
    size_t begin = out->header_read;
    size_t count = headsize - begin;
    ssize_t rs = msgu_stream_read(in, (char *) &out->header, count);
    if (rs <= 0) {
        return rs;
    }
    else {
        out->header_read += rs;
    }
    if (out->header_read == headsize) {
        return 1;
    }
    return 0;
}


void msg_write_header(struct msgu_buffer *b, enum msg_type_id id, int32_t length) {
    struct msgu_header head;
    head.type = id;
    head.size = length;
    ep_buffer_insert(b, (char *) &head, sizeof(head));
}


void msg_req_share(struct msgu_buffer *b, const char *path) {
    struct msgu_header head;
    head.type = msg_type_share_file;
    head.share_id = -1;
    head.size = strlen(path);
    ep_buffer_insert(b, (char *) &head, sizeof(head));
    ep_buffer_insert(b, path, head.size);
}


void msg_req_peer_init(struct msgu_stream *s, struct msg_host *h) {
    struct msgu_header head;
    head.type = msg_type_peer_init;
    head.share_id = -1;
    head.size = 32 + 256 + ep_share_set_size(&h->shares);
    msgu_stream_write(s, (char *) &head, sizeof(head));
    //msg_host_write(h, b);
}


void msg_req_proc_init(struct msgu_stream *s, const char *msg, size_t count) {
    struct msgu_header head;
    head.type = msg_type_share_proc;
    head.share_id = -1;
    head.size = count;
    msgu_stream_write(s, (char *) &head, sizeof(head));
    msgu_stream_write(s, msg, count);
}


size_t msg_send_block(struct msgu_buffer *buf, int share_id, int hdl, char *in, size_t count) {
    struct msgu_header head;
    head.type = msg_type_data;
    head.share_id = share_id;
    head.size = sizeof(share_id) + sizeof(hdl) + count;
    ep_buffer_insert(buf, (char *) &head, sizeof(head));
    ep_buffer_insert(buf, (char *) &share_id, sizeof(share_id));
    ep_buffer_insert(buf, (char *) &hdl, sizeof(hdl));
    ep_buffer_insert(buf, in, count);
}


void msg_send_host(struct msg_host *h, struct msgu_buffer *buf) {
    struct msgu_header head;
    size_t host_count = 1;

    printf("send self\n");
    head.type = msg_type_peer_one;
    head.size = sizeof(size_t);
    head.size += 32 + 256 + ep_share_set_size(&h->shares);
    printf("send %d bytes\n", head.size);
    ep_buffer_insert(buf, (char *) &head, sizeof(struct msgu_header));
    ep_buffer_insert(buf, (char *) &host_count, sizeof(size_t));
    msg_host_write(h, buf);

    if (msg_invalid_buffer(buf)) {
        printf("error sending buffer (%lu should be %d)\n", buf->size, head.size);
    }
}


void msg_send_host_list(struct msg_host_list *h, struct msgu_buffer *buf) {
    struct msgu_header head;

    printf("send %lu hosts\n", h->host_count);
    head.type = msg_type_peer_all;
    head.size = sizeof(size_t);
    for (int i = 0; i < h->host_count; ++i) {
        head.size += 32 + 256 + ep_share_set_size(&h->ptr[i].shares);
    }
    printf("send %d bytes\n", head.size);
    ep_buffer_insert(buf, (char *) &head, sizeof(struct msgu_header));
    ep_buffer_insert(buf, (char *) &h->host_count, sizeof(size_t));
    for (int i = 0; i < h->host_count; ++i) {
        msg_host_write(&h->ptr[i], buf);
    }

    if (msg_invalid_buffer(buf)) {
        printf("error sending buffer (%lu should be %d)\n", buf->size, head.size);
    }
}
