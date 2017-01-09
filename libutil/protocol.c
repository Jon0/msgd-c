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


void msgu_stat_init(struct msgu_read_status *stat) {
    msgu_stat_reset(stat);
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

    // check if read has completed
    if (stat->header_read == headsize) {
        msgu_fragment_inc(&stat->fragment);
        return 1;
    }
    else {
        return 0;
    }
}


int msgu_push_header(struct msgu_stream *out, struct msgu_fragment *f, enum msgu_msgtype id, int32_t length) {
    struct msgu_header head;
    head.type = id;
    head.size = length;
    char *headbuf = (char *) &head;
    ssize_t write_size = msgu_stream_write(out, &headbuf[f->progress], sizeof(head) - f->progress);
    if (write_size > 0) {
        f->progress += write_size;
    }

    // check if write has completed
    if (f->progress == sizeof(head)) {
        msgu_fragment_inc(f);
        return 1;
    }
    else {
        return 0;
    }
}


int msgu_poll_update(struct msgu_stream *in, struct msgu_read_status *stat, union msgu_any_update *update) {
    ssize_t read;
    msgu_fragment_base_zero(&stat->fragment);
    int have_header = msgu_poll_header(in, stat);
    if (have_header <= 0) {
        return have_header;
    }

    // read message
    msgu_fragment_base_inc(&stat->fragment);
    switch (stat->header.type) {
    case msgtype_init_local:
        read = msgu_init_local_read(in, &stat->fragment, &update->init_local);
        break;
    case msgtype_init_remote:
        read = msgu_init_remote_read(in, &stat->fragment, &update->init_remote);
        break;
    case msgtype_add_share:
        read = msgu_add_share_read(in, &stat->fragment, &update->sh_add);
        break;
    default:
        printf("unknown update %d\n", stat->header.type);
        read = -3;
        break;
    }

    if (msgu_fragment_read_check(&stat->fragment, stat->header.size)) {
        // message is completely read
        return stat->header.type;
    }
    else if (read > 0) {
        // socket is still open but waiting
        return 0;
    }
    else {
        return read;
    }
}


int msgu_push_update(struct msgu_stream *out, struct msgu_fragment *f, int update_type, union msgu_any_update *update) {
    msgu_fragment_base_zero(f);
    size_t update_size = msgu_update_size(update_type, update);
    msgu_push_header(out, f, update_type, update_size);
    msgu_fragment_base_inc(f);
    switch (update_type) {
    case msgtype_init_local:
        msgu_init_local_write(out, f, &update->init_local);
        break;
    case msgtype_init_remote:
        msgu_init_remote_write(out, f, &update->init_remote);
        break;
    case msgtype_add_share:
        msgu_add_share_write(out, f, &update->sh_add);
        break;
    default:
        break;
    }
    return 1;
}


// unused
void msg_req_share(struct msgu_buffer *b, const char *path) {
    struct msgu_header head;
    head.type = msgtype_share_file;
    head.share_id = -1;
    head.size = strlen(path);
    ep_buffer_insert(b, (char *) &head, sizeof(head));
    ep_buffer_insert(b, path, head.size);
}


void msg_req_peer_init(struct msgu_stream *s, struct msg_host *h) {
    struct msgu_header head;
    head.type = msgtype_peer_init;
    head.share_id = -1;
    head.size = 32 + 256 + ep_share_set_size(&h->shares);
    msgu_stream_write(s, (char *) &head, sizeof(head));
    //msg_host_write(h, b);
}


void msg_req_proc_init(struct msgu_stream *s, const char *msg, size_t count) {
    struct msgu_header head;
    head.type = msgtype_share_proc;
    head.share_id = -1;
    head.size = count;
    msgu_stream_write(s, (char *) &head, sizeof(head));
    msgu_stream_write(s, msg, count);
}


void msg_send_host(struct msg_host *h, struct msgu_stream *s) {
    struct msgu_header head;
    size_t host_count = 1;
    head.type = msgtype_peer_one;
    head.size = sizeof(size_t);
    head.size += 32 + 256 + ep_share_set_size(&h->shares);
    printf("send %d bytes\n", head.size);
    msgu_stream_write(s, (char *) &head, sizeof(struct msgu_header));
    msgu_stream_write(s, (char *) &host_count, sizeof(size_t));
    msg_host_write(h, s);
}


void msg_send_host_list(struct msg_host_list *h, struct msgu_stream *s) {
    struct msgu_header head;
    head.type = msgtype_peer_all;
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
