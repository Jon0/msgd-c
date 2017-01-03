#include <stdio.h>
#include <string.h>

#include "network.h"


void msgu_channel_init(struct msgu_channel *c, msgu_stream_id_t id, struct msgu_stream_fn *fn) {
    c->mode = 1;
    c->update_type = 0;
    msgu_stream_init(&c->stream, id, fn);
    msgu_stat_init(&c->stat);
}


int msgu_channel_is_local(struct msgu_channel *c) {
    return 1;
}


int msgu_channel_is_closed(struct msgu_channel *c) {
    return (c->mode == 0);
}


int msgu_channel_read(struct msgu_channel *c) {
    if (c->mode == 0) {
        printf("reading closed socket\n");
        return 0;
    }

    c->update_type = msgu_poll_update(&c->stream, &c->stat, &c->update);
    if (c->update_type > 0) {
        return 1;
    }
    else if (c->update_type == -1) {
        c->mode = 0;
        return 0;
    }
    else {
        return 0;
    }
}


int msgu_channel_update_move(struct msgu_channel *c, int *update_type, union msgu_any_update *update) {
    if (c->update_type > 0) {
        memcpy(update_type, &c->update_type, sizeof(int));
        memcpy(update, &c->update, sizeof(union msgu_any_update));
        msgu_stat_reset(&c->stat);
        c->update_type = 0;
        return 1;
    }
    else {
        return 0;
    }
}
