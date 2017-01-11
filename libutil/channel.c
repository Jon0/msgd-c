#include <stdio.h>
#include <string.h>

#include "channel.h"


void msgu_status_reset(struct msgu_status *stat) {
    msgu_fragment_reset(stat->stat, MSGU_FRAGMENT_MAX);
    stat->head.data_type = 0;
    stat->head.size = 0;
}


void msgu_channel_init(struct msgu_channel *c, msgu_stream_id_t id, struct msgu_stream_fn *fn) {
    msgu_stream_init(&c->stream, id, fn);
    msgu_status_reset(&c->read);
    msgu_status_reset(&c->write);
    c->mode = 1;
}


int msgu_channel_is_local(struct msgu_channel *c) {
    return 1;
}


int msgu_channel_is_closed(struct msgu_channel *c) {
    return (c->mode == 0);
}


int msgu_channel_read(struct msgu_channel *c) {
    int result;
    if (c->mode == 0) {
        printf("reading closed socket\n");
        return 0;
    }

    if (c->read.stat[0].index == 0) {
        result = msgu_read_fixed(&c->stream, &c->read.stat[1], &c->read.head, sizeof(c->read.head));
        if (result > 0) {
            msgu_fragment_inc(&c->read.stat[0]);
        }
    }

    if (c->read.stat[0].index == 1) {
        result = msgu_poll_update(&c->stream, &c->read.stat[1], &c->read.head, &c->read.data);
        if (result > 0) {
            msgu_fragment_inc(&c->read.stat[0]);
        }
    }

    if (c->read.stat[0].index == 2) {
        return 1;
    }
    else if (result == -1) {
        c->mode = 0;
        return 0;
    }
    else {
        return 0;
    }
}


int msgu_channel_write(struct msgu_channel *c) {
    int result;
    if (c->mode == 0) {
        printf("writing closed socket\n");
        return 0;
    }
    // check update is ready to be written

    if (c->write.stat[0].index == 0) {
        result = msgu_write_fixed(&c->stream, &c->write.stat[1], &c->write.head, sizeof(c->write.head));
        if (result > 0) {
            msgu_fragment_inc(&c->write.stat[0]);
        }
    }

    if (c->write.stat[0].index == 1) {
        result = msgu_push_update(&c->stream, &c->write.stat[1], &c->write.head, &c->write.data);
        if (result > 0) {
            msgu_fragment_inc(&c->write.stat[0]);
        }
    }

    // free update when completed
    if (c->write.stat[0].index == 2) {
        return 1;
    }
    else if (result == -1) {
        c->mode = 0;
        return 0;
    }
    else {
        return 0;
    }
}


int msgu_channel_update_move(struct msgu_channel *c, int *update_type, union msgu_any_update *update) {
    if (c->read.head.data_type > 0) {
        memcpy(update_type, &c->read.head.data_type, sizeof(int));
        memcpy(update, &c->read.data, sizeof(union msgu_any_update));
        msgu_fragment_reset(c->read.stat, MSGU_FRAGMENT_MAX);
        c->read.head.data_type = 0;
        return 1;
    }
    else {
        return 0;
    }
}


int msgu_channel_update_send(struct msgu_channel *c, int update_type, union msgu_any_update *update) {
    msgu_fragment_reset(c->write.stat, MSGU_FRAGMENT_MAX);
    c->write.head.data_type = update_type;
    c->write.head.size = msgu_update_size(update_type, update);
    c->write.data = *update;
    return 1;
}


int msgu_poll_update(struct msgu_stream *in, struct msgu_fragment *f, struct msgu_channel_header *h, union msgu_any_update *data) {
    int read;
    switch (h->data_type) {
    case msgtype_init_local:
        read = msgu_init_local_read(in, f, &data->init_local);
        break;
    case msgtype_init_remote:
        read = msgu_init_remote_read(in, f, &data->init_remote);
        break;
    case msgtype_add_share_file:
        read = msgu_share_file_read(in, f, &data->share_file);
        break;
    default:
        printf("unknown update %d\n", h->data_type);
        read = -3;
        break;
    }
    return msgu_fragment_complete(f, read, h->size);
}


int msgu_push_update(struct msgu_stream *out, struct msgu_fragment *f, struct msgu_channel_header *h, union msgu_any_update *data) {
    int written;
    switch (h->data_type) {
    case msgtype_init_local:
        written = msgu_init_local_write(out, f, &data->init_local);
        break;
    case msgtype_init_remote:
        written = msgu_init_remote_write(out, f, &data->init_remote);
        break;
    case msgtype_add_share_file:
        written = msgu_share_file_write(out, f, &data->share_file);
        break;
    default:
        printf("unknown update %d\n", h->data_type);
        written = -3;
        break;
    }
    return msgu_fragment_complete(f, written, h->size);
}
