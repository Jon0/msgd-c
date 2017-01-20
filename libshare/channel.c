#include <stdio.h>
#include <string.h>

#include "channel.h"


void msgu_status_reset(struct msgu_status *stat, int ready) {
    msgu_fragment_reset(stat->stat, MSGU_FRAGMENT_MAX);
    stat->head.data_type = 0;
    stat->head.size = 0;
    stat->ready = ready;
}


void msgu_channel_init(struct msgu_channel *c, msgu_stream_id_t id, struct msgu_stream_fn *fn) {
    msgu_stream_init(&c->stream, id, fn);
    msgu_status_reset(&c->read, 0);
    msgu_status_reset(&c->write, 1);
    c->mode = 1;
}


void msgu_channel_free(struct msgu_channel *c) {

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

    // check update is ready to be read
    if (c->read.ready) {
        return 1;
    }

    if (c->read.stat[0].index == 0) {
        result = msgu_read_fixed(&c->stream, &c->read.stat[1], &c->read.head, sizeof(c->read.head));
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(&c->read.stat[0]);
        }
    }

    if (c->read.stat[0].index == 1) {
        result = msgu_any_update_read(&c->stream, &c->read.stat[1], c->read.head.data_type, &c->read.data);
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(&c->read.stat[0]);
        }
    }

    // check if message transfer completed
    if (c->read.stat[0].index == 2) {
        size_t hsize = c->read.head.size;
        size_t asize = c->read.stat[0].count - sizeof(c->read.head);
        if (hsize == asize) {
            c->read.ready = 1;
        }
        else {
            printf("incorrect message, header: %d, actual: %d\n", hsize, asize);
            c->mode = 0;
        }
        msgu_fragment_reset(c->read.stat, MSGU_FRAGMENT_MAX);
    }
    if (result <= msgu_stream_remote_closed) {
        c->mode = 0;
    }
    return (result >= msgu_stream_partial);
}


int msgu_channel_write(struct msgu_channel *c) {
    int result;
    if (c->mode == 0) {
        printf("writing closed socket\n");
        return 0;
    }

    // check update is ready to be written
    if (c->write.ready) {
        return 1;
    }


    if (c->write.stat[0].index == 0) {
        result = msgu_write_fixed(&c->stream, &c->write.stat[1], &c->write.head, sizeof(c->write.head));
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(&c->write.stat[0]);
        }
    }

    if (c->write.stat[0].index == 1) {
        result = msgu_any_update_write(&c->stream, &c->write.stat[1], c->write.head.data_type, &c->write.data);
        if (result == msgu_stream_complete) {
            msgu_fragment_inc(&c->write.stat[0]);
        }
    }

    // check if message transfer completed
    // TODO: free update when completed
    if (c->write.stat[0].index == 2) {
        msgu_fragment_reset(c->write.stat, MSGU_FRAGMENT_MAX);
        c->write.ready = 1;
    }

    if (result <= msgu_stream_remote_closed) {
        c->mode = 0;
    }
    return (result >= msgu_stream_partial);
}


int msgu_channel_update_move(struct msgu_channel *c, int *update_type, union msgu_any_update *update) {
    if (c->read.ready) {
        c->read.ready = 0;
        memcpy(update_type, &c->read.head.data_type, sizeof(int));
        memcpy(update, &c->read.data, sizeof(union msgu_any_update));
        return 1;
    }
    else {
        return 0;
    }
}


int msgu_channel_update_send(struct msgu_channel *c, int update_type, union msgu_any_update *update) {
    if (c->write.ready) {
        c->write.ready = 0;
        c->write.head.data_type = update_type;
        c->write.head.size = msgu_update_size(update_type, update);
        c->write.data = *update;
        return 1;
    }
    else {
        return 0;
    }
}


int msgu_channel_try_read(struct msgu_channel *c, int *update_type, union msgu_any_update *update) {
    while (msgu_channel_read(c)) {
        if (msgu_channel_update_move(c, update_type, update)) {
            return 1;
        }
    }
    return 0;
}
