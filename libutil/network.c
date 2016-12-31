#include "network.h"


int msgu_channel_read(struct msgu_channel *c, union msgu_any_update *u) {
    int status = msgu_poll_header(&c->stream, &c->stat);
    if (status > 0) {
        //
        msgu_stat_reset(&c->stat);
    }
    return status;
}


int msgu_channel_is_local(struct msgu_channel *c) {
    return 1;
}
