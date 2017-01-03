#include "network.h"


void msgu_channel_init(struct msgu_channel *c, msgu_stream_id_t id, struct msgu_stream_fn *fn) {
    msgu_stream_init(&c->stream, id, fn);
    msgu_stat_init(&c->stat);
}


int msgu_channel_is_local(struct msgu_channel *c) {
    return 1;
}


int msgu_channel_is_closed(struct msgu_channel *c) {
    return 0;
}


int msgu_channel_read(struct msgu_channel *c) {
    int status = msgu_poll_header(&c->stream, &c->stat);
    if (status > 0) {
        c->update_type = msgu_poll_update(&c->stream, &c->stat, &c->update);
        if (c->update_type > 0) {
            msgu_channel_update_free(c);
            msgu_stat_reset(&c->stat);
        }
    }
    return status;
}


int msgu_channel_update_copy(struct msgu_channel *c, int *update_type, union msgu_any_update *update) {

}


int msgu_channel_update_free(struct msgu_channel *c) {
    msgu_update_free(c->update_type, &c->update);
}
