#include "notify.h"


void msg_notify_map_init(struct msg_notify_map *n, size_t size) {
    static size_t value_sizes [] = {
        sizeof(int),
        sizeof(struct msg_notify_state),
    };
    msgu_datatable_init(&n->data, value_sizes, sizeof(value_sizes) / sizeof(size_t));
    msgu_datatable_alloc(&n->data, size);
    msgu_datamap_init(&n->id_map, &n->data, 0, msgu_int_hash, msgu_int_cmp);
    msgu_datamap_alloc(&n->id_map, size);
}


int msg_notify_map_add(struct msg_notify_map *n, int msg_id, msg_notify_fn_t fn, void *arg) {
    void *datarow[2];
    int index = msgu_datatable_emplace(&n->data, datarow);

    // set initial state
    *((int *) datarow[0]) = msg_id;
    struct msg_notify_state *ns = datarow[1];
    ns->state = 0;
    ns->fn = fn;
    ns->arg = arg;
    msgu_datamap_create_key(&n->id_map, index);
    return index;
}


int msg_notify_map_signal(struct msg_notify_map *n, struct msg_connection *conn, const struct msgu_message *msg) {
    void *datarow[2];
    int msg_id = msg->event_id;
    int index = msgu_datamap_index(&n->id_map, &msg_id, datarow);
    if (index < 0) {
        return -1;
    }
    else {
        struct msg_notify_state *ns = datarow[1];
        return ns->fn(ns->arg, conn, msg);
    }
}
