#include <stdio.h>

#include "hostlist.h"


void msgu_host_list_init(struct msg_host_list *list, size_t size, msg_message_recv_t rf, void *arg) {
    static size_t value_sizes [] = {
        sizeof(int),
        sizeof(struct msgu_address),
        sizeof(struct msgu_string),
        sizeof(struct msg_host_link),
    };
    list->recv_fn = rf;
    list->recv_arg = arg;
    msgs_mutex_init(&list->list_mutex);
    msgu_datatable_init(&list->data, value_sizes, sizeof(value_sizes) / sizeof(size_t));
    msgu_datatable_alloc(&list->data, size);
    msgu_datamap_init(&list->id_map, &list->data, 0, msgu_int_hash, msgu_int_cmp);
    msgu_datamap_init(&list->name_map, &list->data, 1, msgu_string_hash, msgu_string_cmp);
    msgu_datamap_alloc(&list->id_map, size);
    msgu_datamap_alloc(&list->name_map, size);
}


int msg_hostlist_init_connection(struct msg_host_list *list, struct msgs_event_map *emap, struct msgs_socket *socket) {
    struct msg_connection conn;
    void *datarow [4];
    msg_connection_init(&conn, socket, list->recv_fn, list->recv_arg);

    // TODO check for existing entries
    int event_id = msgu_add_recv_handler(emap);

    // lock and modify server state
    msgs_mutex_lock(&list->list_mutex);
    int index = msgu_datatable_emplace(&list->data, datarow);

    // assign id, addr, name and connection
    *((int *) datarow[0]) = event_id;
    msgs_get_address(socket, datarow[1]);
    msgu_string_init(datarow[2]);
    struct msg_host_link *link = datarow[3];
    link->status_bits = msg_host_active;
    link->conn = conn;

    msgs_mutex_unlock(&list->list_mutex);
    return event_id;
}


int msg_hostlist_close_connection(struct msg_host_list *list, int id) {
    struct msg_host_link *link = msg_hostlist_connection_id(list, id);
    if (link) {
        msgs_mutex_lock(&list->list_mutex);
        if (link->status_bits & msg_host_active) {
            msg_connection_close(&link->conn);
            link->status_bits = 0;
        }
        msgs_mutex_unlock(&list->list_mutex);
        return 1;
    }
    else {
        return -1;
    }
}


struct msg_host_link *msg_hostlist_connection_id(struct msg_host_list *list, int id) {
    void *datarow [4];
    size_t index = 0;
    int result = msgu_datatable_get(&list->data, datarow, index);
    if (result == -1) {
        printf("Host id: %lu not found\n", index);
        return NULL;
    }
    return datarow[3];
}


struct msg_host_link *msg_hostlist_connection_name(struct msg_host_list *list, const struct msgu_string *hostname) {
    void *datarow [4];
    size_t index = 0;
    int result = msgu_datatable_get(&list->data, datarow, index);
    if (result == -1) {
        printf("Host id: %lu not found\n", index);
        return NULL;
    }
    return datarow[3];
}


int msg_hostlist_connection_notify(struct msg_host_list *list, int id) {
    struct msg_host_link *link = msg_hostlist_connection_id(list, id);

    // make sure every path unlocks this
    msgs_mutex_lock(&list->list_mutex);
    if (link) {
        msg_connection_notify(&link->conn);

        // lock read mutex first
        if (msgs_mutex_try_lock(&link->conn.read_mutex)) {
            msgs_mutex_unlock(&list->list_mutex);
            int result = msg_connection_poll(&link->conn);
            msgs_mutex_unlock(&link->conn.read_mutex);
            if (result == -1) {
                // socket was closed
                printf("connection %d: closed\n", id);
                msg_hostlist_close_connection(list, id);
            }
        }
        else {
            msgs_mutex_unlock(&list->list_mutex);
        }
    }
    else {
        printf("connection %d: not found\n", id);
        msgs_mutex_unlock(&list->list_mutex);
    }
}


struct msg_connection *msg_hostlist_use_id(struct msg_host_list *list, msgs_mutex_t **lock, int id) {
    msgs_mutex_lock(&list->list_mutex);
    struct msg_host_link *link = msg_hostlist_connection_id(list, id);
    if (link && (link->status_bits & msg_host_active)) {
        msgs_mutex_lock(&link->conn.read_mutex);
        msgs_mutex_unlock(&list->list_mutex);
        *lock = &link->conn.read_mutex;
        return &link->conn;
    }
    else {
        msgs_mutex_unlock(&list->list_mutex);
        return NULL;
    }
}
