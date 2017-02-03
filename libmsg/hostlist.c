#include "hostlist.h"


void msgu_host_list_init(struct msg_host_list *l, size_t size) {
    static size_t value_sizes [] = {
        sizeof(int),
        sizeof(struct msgu_address),
        sizeof(struct msgu_string),
        sizeof(struct msg_host_link),
    };
    msgs_mutex_init(&l->list_mutex);
    msgu_datatable_init(&l->data, value_sizes, sizeof(value_sizes) / sizeof(size_t));
    msgu_datatable_alloc(&l->data, size);
    msgu_datamap_init(&l->id_map, &l->data, 0, msgu_int_hash, msgu_int_cmp);
    msgu_datamap_init(&l->name_map, &l->data, 1, msgu_string_hash, msgu_string_cmp);
    msgu_datamap_alloc(&l->id_map, size);
    msgu_datamap_alloc(&l->name_map, size);
}
