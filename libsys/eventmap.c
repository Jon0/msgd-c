#include <stdio.h>
#include <string.h>

#include "eventmap.h"

void msgu_event_map_init(struct msgs_event_map *map, struct msgu_handlers *h, void *a) {
    pthread_mutex_init(&map->map_mutex, NULL);
    msgu_map_init(&map->data, msgu_uint32_hash, msgu_uint32_cmp, sizeof(uint32_t), sizeof(union msgu_any_event));
    msgu_map_alloc(&map->data, 256);
    map->next_id = 1;
    map->hdl     = *h;
    map->arg     = a;
}


int msgu_event_copy(struct msgs_event_map *map, uint32_t id, union msgu_any_event *data) {
    union msgu_any_event *any = msgu_map_get(&map->data, &id);
    if (any) {
        memcpy(data, any, sizeof(union msgu_any_event));
        return 1;
    }
    else {
        return 0;
    }
}


void msgu_event_notify(struct msgs_event_map *map, uint32_t type, union msgu_any_event *data) {
    switch (type) {
    case msgu_connect_id:
        map->hdl.connect_event(map->arg, &data->conn);
        break;
    case msgu_recv_id:
        map->hdl.recv_event(map->arg, &data->recv);
        break;
    }
}


void msgs_event_recv(struct msgs_event_map *map, uint32_t ev, uint32_t type, uint32_t id) {
    union msgu_any_event data;
    pthread_mutex_lock(&map->map_mutex);
    int count = msgu_event_copy(map, id, &data);
    pthread_mutex_unlock(&map->map_mutex);
    if (count) {
        msgu_event_notify(map, type, &data);
    }
    else {
        printf("event %d, %d not found\n", type, id);
    }
}


int msgu_add_connect_handler(struct msgs_event_map *map) {
    struct msgu_connect_event ce;
    pthread_mutex_lock(&map->map_mutex);
    uint32_t id = map->next_id++;
    ce.id = id;
    int count = msgu_map_insert(&map->data, &id, &ce);
    if (count == 0) {
        return -1;
    }
    pthread_mutex_unlock(&map->map_mutex);
    return id;

}


int msgu_add_recv_handler(struct msgs_event_map *map) {
    struct msgu_recv_event re;
    pthread_mutex_lock(&map->map_mutex);
    uint32_t id = map->next_id++;
    re.id = id;
    msgu_map_insert(&map->data, &id, &re);
    pthread_mutex_unlock(&map->map_mutex);
    return id;
}


int msgu_add_share_handler(struct msgs_event_map *map) {
    struct msgu_share_event fe;
    pthread_mutex_lock(&map->map_mutex);
    uint32_t id = map->next_id++;
    fe.id = id;
    msgu_map_insert(&map->data, &id, &fe);
    pthread_mutex_unlock(&map->map_mutex);
    return id;
}


int msgu_add_mount_handler(struct msgs_event_map *map) {
    struct msgu_mount_event me;
    pthread_mutex_lock(&map->map_mutex);
    uint32_t id = map->next_id++;
    me.id = id;
    msgu_map_insert(&map->data, &id, &me);
    pthread_mutex_unlock(&map->map_mutex);
    return id;
}


int msgu_remove_handler(struct msgs_event_map *map, uint32_t id) {
    pthread_mutex_lock(&map->map_mutex);
    int result = msgu_map_erase(&map->data, &id);
    pthread_mutex_unlock(&map->map_mutex);
    return result;
}
