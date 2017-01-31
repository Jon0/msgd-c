#include "hostlist.h"


void msgu_host_list_init(struct msg_host_list *l) {
    static size_t value_sizes [] = {
        sizeof(struct msgu_string),
        sizeof(struct msg_host_link),
    };
    msgs_mutex_init(&l->list_mutex);
    msgu_datatable_init(&l->data, value_sizes, sizeof(value_sizes) / sizeof(size_t));
}
