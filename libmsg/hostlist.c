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
    msgu_datamap_init(&list->addr_map, &list->data, 1, msgu_address_hash, msgu_address_cmp);
    msgu_datamap_init(&list->name_map, &list->data, 2, msgu_string_hash, msgu_string_cmp);
    msgu_datamap_alloc(&list->id_map, size);
    msgu_datamap_alloc(&list->addr_map, size);
    msgu_datamap_alloc(&list->name_map, size);
}


int msg_hostlist_init_connection(struct msg_host_list *list, struct msgs_event_map *emap, struct msgs_socket *socket) {
    struct msgu_address addr;
    void *datarow [4];
    msgs_get_address(socket, &addr);
    int index = msgu_datamap_index(&list->addr_map, &addr, datarow);
    int event_id;


    // lock and modify server state
    msgs_mutex_lock(&list->list_mutex);

    // check for existing entries by address
    if (index < 0) {

        // new entry:  assign id, addr, name and connection
        index = msgu_datatable_emplace(&list->data, datarow);
        event_id = msgu_add_recv_handler(emap);
        *((int *) datarow[0]) = event_id;
        msgs_get_address(socket, datarow[1]);
        msgu_string_init(datarow[2]);
        struct msg_host_link *link = datarow[3];
        link->status_bits = msg_host_active;
        msg_connection_init(&link->conn, event_id, socket, list->recv_fn, list->recv_arg);
        msgu_datamap_create_key(&list->id_map, index);
        msgu_datamap_create_key(&list->addr_map, index);
    }
    else {

        // reconnecting
        event_id = *((int *) datarow[0]);
        struct msg_host_link *link = datarow[3];
        link->status_bits |= msg_host_active;
        msg_connection_init(&link->conn, event_id, socket, list->recv_fn, list->recv_arg);
    }

    msgs_mutex_unlock(&list->list_mutex);
    return event_id;
}


int msg_hostlist_close_connection(struct msg_host_list *list, int id) {
    struct msg_host_link *link = msg_hostlist_connection_id(list, id);
    if (link) {
        msgs_mutex_lock(&list->list_mutex);
        if (link->status_bits & msg_host_active) {
            msg_connection_close(&link->conn);
            link->status_bits &= ~msg_host_active;
        }
        msgs_mutex_unlock(&list->list_mutex);
        return 1;
    }
    else {
        return -1;
    }
}


int msg_hostlist_name_connection(struct msg_host_list *list, int id, const struct msgu_string *name) {
    void *datarow [4];
    int index = msgu_datamap_index(&list->id_map, &id, datarow);
    if (index < 0) {
        printf("Host id: %lu not found\n", index);
        return -1;
    }
    else {
        struct msg_host_link *link = datarow[3];
        msgu_string_copy(datarow[2], name);
        msg_connection_set_name(&link->conn, datarow[2]);
        msgu_datamap_create_key(&list->name_map, index);
        return index;
    }
}


void msg_hostlist_print(struct msg_host_list *list) {
    void *datarow [4];
    int id;
    char addr [128];
    char status [128];
    struct msgu_string *name;
    struct msg_host_link *link;
    printf("ID\tAddr\tName\tStat\n");
    for (int i = 0; i < list->data.row_count; ++i) {
        msgu_datatable_get(&list->data, datarow, i);
        id = *((int *) datarow[0]);
        msgs_address_print(addr, datarow[1]);
        name = datarow[2];
        link = datarow[3];
        if (link->status_bits & msg_host_active) {
            sprintf(status, "Active");
        }
        else {
            sprintf(status, "Inactive");
        }
        printf("%d\t%s\t%s\t%s\n", id, addr, name->buf, status);
    }
}


struct msg_host_link *msg_hostlist_connection_id(struct msg_host_list *list, int id) {
    void *datarow [4];
    int index = msgu_datamap_index(&list->id_map, &id, datarow);
    if (index < 0) {
        printf("Host id: %d not found\n", index);
        return NULL;
    }
    return datarow[3];
}


struct msg_host_link *msg_hostlist_connection_name(struct msg_host_list *list, const struct msgu_string *hostname) {
    void *datarow [4];
    int index = msgu_datamap_index(&list->name_map, hostname, datarow);
    if (index < 0) {
        printf("Host id: %d not found\n", index);
        return NULL;
    }
    return datarow[3];
}


int msg_hostlist_connection_notify(struct msg_host_list *list, int id) {

    // make sure every path unlocks this
    msgs_mutex_lock(&list->list_mutex);
    struct msg_host_link *link = msg_hostlist_connection_id(list, id);
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


struct msg_connection *msg_hostlist_use_host(struct msg_host_list *list, msgs_mutex_t **lock, const struct msgu_string *hostname) {
    msgs_mutex_lock(&list->list_mutex);
    struct msg_host_link *link = msg_hostlist_connection_name(list, hostname);
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
