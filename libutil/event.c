#include <string.h>

#include "event.h"


int msgu_local_connect(struct msgu_connect_event *ce) {
    return 0;
}


size_t msgu_recv_event_callback(struct msgu_recv_event *re, struct msgu_buffer *b, void *p) {
    return 0;
}


void msgu_event_map_init(struct msgu_event_map *map, struct msgu_handlers *h, void *a) {
    msgu_map_init(&map->data, msgu_uint32_hash, msgu_uint32_cmp, sizeof(uint32_t), sizeof(union msgu_any_event));
    msgu_map_alloc(&map->data, 256);
    map->next_id = 1;
    map->hdl = *h;
    map->arg = a;
}


int msgu_event_copy(struct msgu_event_map *map, uint32_t id, union msgu_any_event *data) {
    union msgu_any_event *any = msgu_map_get(&map->data, &id);
    if (any) {
        memcpy(data, any, sizeof(union msgu_any_event));
        return 1;
    }
    else {
        return 0;
    }
}


void msgu_event_notify(struct msgu_event_map *map, uint32_t type, union msgu_any_event *data) {
    switch (type) {
    case msgu_connect:
        map->hdl.connect_event(map->arg, &data->conn);
        break;
    case msgu_recv:
        map->hdl.recv_event(map->arg, &data->recv);
        break;
    }
}


int msgu_add_conn(struct msgu_event_map *map, struct msgu_connect_event *ce) {
    uint32_t id = map->next_id++;
    ce->id = id;
    msgu_map_insert(&map->data, &id, ce);
    return id;
}


int msgu_add_recv(struct msgu_event_map *map, struct msgu_recv_event *re) {
    uint32_t id = map->next_id++;
    re->id = id;
    msgu_map_insert(&map->data, &id, re);
    return id;
}


int msgu_add_file(struct msgu_event_map *map, struct msgu_file_event *fe) {
    uint32_t id = map->next_id++;
    fe->id = id;
    msgu_map_insert(&map->data, &id, fe);
    return id;
}


int msgu_remove(struct msgu_event_map *map, uint32_t id) {
    return msgu_map_erase(&map->data, &id);
}
