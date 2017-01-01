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


void msgu_stat_reset(struct msgu_read_status *stat) {
    stat->header_read = 0;
    stat->message_read = 0;
}


int msgu_poll_header(struct msgu_stream *in, struct msgu_read_status *stat) {

    // recieving requests to the local server
    char *buf = (char *) &stat->header;
    size_t headsize = sizeof(struct msgu_header);
    size_t begin = stat->header_read;
    size_t count = headsize - begin;
    if (count == 0) {
        return 1;
    }
    ssize_t rs = msgu_stream_read(in, (char *) &stat->header, count);
    if (rs <= 0) {
        return rs;
    }
    else {
        stat->header_read += rs;
    }
    if (stat->header_read == headsize) {
        return 1;
    }
    return 0;
}


int msgu_poll_update(struct msgu_stream *in, struct msgu_read_status *stat, union msgu_any_update *update) {
    ssize_t read;
    struct msgu_header *head = &stat->header;
    switch (head->type) {
    case msg_type_share_file:
        read = msgu_add_share(in, &update->sh_add, stat->message_read);
        break;
    default:
        read = -1;
        break;
    }

    // check socket is still open
    if (read > 0) {
        stat->message_read += read;
    }
    else if (read < 0) {
        return -1;
    }


    // check message was completely read
    if (stat->header.size == stat->message_read) {
        return head->type;
    }
    else {
        return 0;
    }
}


ssize_t msgu_add_peer(struct msgu_stream *in, struct msgu_add_peer_update *u, size_t offset) {
    return 0;
}


ssize_t msgu_add_share(struct msgu_stream *in, struct msgu_add_share_update *as, size_t offset) {
    return msgu_string_read(&as->share_name, in, offset);
}


ssize_t msgu_header_write(struct msgu_stream *out, enum msg_type_id id, int32_t length, size_t offset) {
    struct msgu_header head;
    head.type = id;
    head.size = length;
    char *headbuf = (char *) &head;
    return msgu_stream_write(out, &headbuf[offset], sizeof(head) - offset);
}


ssize_t msgu_add_peer_write(struct msgu_stream *out, struct msgu_add_peer_update *u, size_t offset) {
    return 0;
}


ssize_t msgu_add_share_write(struct msgu_stream *out, struct msgu_add_share_update *as, size_t offset) {
    if (msgu_header_write(out, msg_type_add_share, msgu_string_size(&as->share_name), offset)); 
    return msgu_string_write(&as->share_name, out, offset);
}


// unused
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


void msg_send_host(struct msg_host *h, struct msgu_stream *s) {
    struct msgu_header head;
    size_t host_count = 1;
    head.type = msg_type_peer_one;
    head.size = sizeof(size_t);
    head.size += 32 + 256 + ep_share_set_size(&h->shares);
    printf("send %d bytes\n", head.size);
    msgu_stream_write(s, (char *) &head, sizeof(struct msgu_header));
    msgu_stream_write(s, (char *) &host_count, sizeof(size_t));
    msg_host_write(h, s);
}


void msg_send_host_list(struct msg_host_list *h, struct msgu_stream *s) {
    struct msgu_header head;
    head.type = msg_type_peer_all;
    head.size = sizeof(size_t);
    for (int i = 0; i < h->host_count; ++i) {
        head.size += 32 + 256 + ep_share_set_size(&h->ptr[i].shares);
    }
    msgu_stream_write(s, (char *) &head, sizeof(struct msgu_header));
    msgu_stream_write(s, (char *) &h->host_count, sizeof(size_t));
    for (int i = 0; i < h->host_count; ++i) {
        msg_host_write(&h->ptr[i], s);
    }
}
