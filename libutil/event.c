#include "event.h"


int msgu_local_connect(struct msgu_connect_event *ce) {
    return 0;
}


size_t msgu_recv_event_callback(struct msgu_recv_event *re, struct msgu_buffer *b, void *p) {
    return 0;
}


void msgu_event_map_init(struct msgu_event_map *map, struct msgu_handlers *h, void *a) {
    map->hdl = *h;
    map->arg = a;
}


void msgu_event_copy(struct msgu_event_map *map, uint32_t type, uint32_t id, void *data) {

}


void msgu_event_notify(struct msgu_event_map *map, uint32_t type, void *data) {

}


int msgu_add_conn(struct msgu_event_map *map, struct msgu_connect_event *ce) {
    return 0;
}


int msgu_add_recv(struct msgu_event_map *map, struct msgu_recv_event *re) {
    return 0;
}


int msgu_add_file(struct msgu_event_map *map, struct msgu_file_event *fe) {
    return 0;
}


int msgu_remove(struct msgu_event_map *map, int id) {
    return 0;
}
