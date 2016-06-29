#include "channel.h"


void msg_channel_run(struct msg_channel_state *s) {

    struct ep_table tb;
    ep_table_init(&tb, "");


    // wait until threads complete
    ep_table_join(&tb);
    ep_table_free(&tb);
}
