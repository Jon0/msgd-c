#ifndef PROTOCOL_H
#define PROTOCOL_H

/*
 * types of messages
 */
enum msg_type_id {
    msg_type_init,
    msg_type_join,
    msg_type_publ,
    msg_type_subs,
    msg_type_avail,
    msg_type_availp,
    msg_type_avails,
    msg_type_poll,
    msg_type_push
};


struct msg_block {
    enum msg_type_id  id;
    char              data [244];
};

#endif
