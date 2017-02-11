#include "notify.h"

void msg_notify_map_init(struct msg_notify_map *n) {
    static size_t value_sizes [] = {
        sizeof(int),
        sizeof(int),
        sizeof(msgs_condition_t),
    };
    msgu_datatable_init(&n->data, value_sizes, sizeof(value_sizes) / sizeof(size_t));
}


int msg_notify_map_add(struct msg_notify_map *n, int msg_id) {

}


int msg_notify_map_wait(struct msg_notify_map *n, int msg_id) {

}
