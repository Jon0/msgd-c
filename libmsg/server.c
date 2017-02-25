#include <stdio.h>
#include <string.h>

#include "server.h"


int msg_server_message_recv(struct msg_connection *conn, struct msgu_message *msg, void *serv) {
    return msg_server_recv(serv, conn, msg);
}


void msg_server_init(struct msg_server *serv, struct msgs_table *tb) {
    msg_notify_map_init(&serv->notify, 32);
    msg_host_list_init(&serv->hostlist, 32, msg_server_message_recv, serv);
    msgu_share_set_init(&serv->shares, &file_ops);
    msgu_mount_map_init(&serv->mounts, 32);
    msgs_file_cache_init(&serv->cache, &serv->shares);
    msgs_host_init_self(&serv->self);
    serv->tb = tb;
    serv->msg_id = 1;
}


int msg_server_accept(struct msg_server *serv, struct msgs_socket *socket) {
    int newid = msg_hostlist_init_connection(&serv->hostlist, serv->tb->emap, socket);
    msgs_poll_socket(serv->tb, socket, newid);
    return newid;
}


int msg_server_connect(struct msg_server *serv, const char *addr) {
    msgs_mutex_t *lock;
    struct msgu_address raddr;
    struct msgs_socket socket;
    struct msgu_init_remote_msg init_msg;
    init_msg.version_maj = 0;
    init_msg.version_min = 1;
    msgu_string_from_static(&init_msg.host_name, serv->self.hostname);

    // try connecting
    msgu_init_remote_addr(&raddr, addr, 2204);
    int fd = msgs_open_socket(&socket, &raddr);
    if (fd == -1) {
        return -1;
    }
    int cid = msg_hostlist_init_connection(&serv->hostlist, serv->tb->emap, &socket);
    struct msg_connection *conn = msg_hostlist_use_id(&serv->hostlist, &lock, cid);
    if (conn) {
        msg_connection_send_message(conn, serv->msg_id++, msgtype_init_remote, msgdata_init_remote, (union msgu_any_msg *) &init_msg);
        msgs_mutex_unlock(lock);
        msgs_poll_socket(serv->tb, &socket, cid);
    }
    else {
        printf("failed to add connection\n");
        return -1;
    }
    return cid;
}


void msg_server_print_state(struct msg_server *serv) {
    printf("[server state]\n");
    msg_hostlist_print(&serv->hostlist);
    msgu_share_debug(&serv->shares);
}


void msg_server_init_mount(struct msg_server *serv, const struct msgu_string *host, const struct msgu_string *share) {
    printf("mounting %s::%s\n", host->buf, share->buf);

    struct msg_host_link *link = msg_hostlist_connection_name(&serv->hostlist, host);
    if (link) {
        printf("found: %s\n", host->buf);

        // TODO use array of available shares
        struct msgu_node nd;
        nd.node_type = 2;
        nd.node_mode = 7;
        nd.node_size = 4096;
        msgu_string_copy(&nd.node_name, share);

        // check connection is active
        if (link->status_bits & msg_host_active) {
            msgu_mount_add(&serv->mounts, host, &nd);
        }
    }
    else {
        printf("not found: %s\n", host->buf);
    }
}


int msg_server_recv(struct msg_server *serv, struct msg_connection *conn, struct msgu_message *msg) {
    msg_connection_log(conn, msg, "recv");
    msg_server_apply(serv, conn, msg);
    msgu_message_free(msg);
    msg_server_print_state(serv);
    return 0;
}


int msg_server_apply(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg) {
    struct msg_status status;
    status.error = 0;
    status.new_handle = 0;
    if (msg_server_validate(serv, conn, msg)) {
        if (msg_server_modify(serv, conn, msg, &status)) {
            msg_server_notify(serv, conn, msg, &status);
            msg_server_reply(serv, conn, msg, &status);
        }
        else {
            return -1;
        }
    }
    return 0;
}


int msg_server_validate(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg) {
    // check update is valid
    return 1;
}


int msg_server_modify(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, struct msg_status *status) {
    int result;

    // lock mutex and apply state changes
    switch (msg->event_type) {
    case msgtype_peer_connect:
        status->error = msg_server_connect(serv, msg->buf.data.host_addr.address_str.buf);
        break;
    case msgtype_init_local:
        msg_hostlist_name_connection(&serv->hostlist, conn->connection_id, &msg->buf.data.init_local.proc_name);
        break;
    case msgtype_init_remote:
    case msgtype_init_remote_reply:
        msg_hostlist_name_connection(&serv->hostlist, conn->connection_id, &msg->buf.data.init_remote.host_name);
        break;
    case msgtype_mount:
        msg_server_init_mount(serv, &msg->buf.data.mount_node.host_name, &msg->buf.data.mount_node.share_name);
        break;
    case msgtype_add_share_file:
        msgu_share_file(&serv->shares, &msg->buf.data.share_file.share_name);
        break;
    case msgtype_file_open:
        status->new_handle = msg_connection_init_handle(conn, &serv->cache, &msg->buf.data.share_file.share_name);
        break;
    case msgtype_file_stream_read:
        status->new_handle = msg->buf.data.node_read.node_handle;
        result = msg_connection_read_handle(conn, &serv->cache, msg->buf.data.node_read.node_handle, status->buf, sizeof(status->buf));
        if (result > 0) {
            status->count = result;
        }
        else {
            status->count = 0;
        }
        break;
    case msgtype_return_node_handle:
        msgu_mount_set_handle(&serv->mounts, &conn->remote_name, msg->buf.data.node_handle.node_handle);
        break;
    default:
     printf("Modify not implemented\n");
    }
    return 1;
}


int msg_server_notify(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, const struct msg_status *status) {
    // send notification to any update listeners
    msg_notify_map_signal(&serv->notify, conn, msg);
    return 1;
}


int msg_server_reply(struct msg_server *serv, struct msg_connection *conn, const struct msgu_message *msg, const struct msg_status *status) {
    int have_update = 0;
    int event_type;
    int data_type;
    union msgu_any_msg data;


    switch (msg->event_type) {
    case msgtype_init_remote:
        have_update = 1;
        event_type = msgtype_init_remote_reply;
        data_type = msgdata_init_remote;
        data.init_remote.version_maj = 0;
        data.init_remote.version_min = 1;
        msgu_string_from_static(&data.init_remote.host_name, serv->self.hostname);
        break;
    case msgtype_list_shares:
        have_update = 1;
        event_type = msgtype_return_share_list;
        data_type = msgdata_node_list;
        msgs_node_list_of_shares(&serv->shares, &data.node_list.nodes);
        break;
    case msgtype_file_open:
        have_update = 1;
        event_type = msgtype_return_node_handle;
        data_type = msgdata_node_handle;
        data.node_handle.node_handle = status->new_handle;
        break;
    case msgtype_file_stream_read:
        have_update = 1;
        event_type = msgtype_return_node_content;
        data_type = msgdata_node_write;
        data.node_write.node_handle = status->new_handle;
        msgu_string_from_buffer(&data.node_write.data, status->buf, status->count);
        break;
    }

    if (have_update) {
        msg_connection_send_message(conn, msg->event_id, event_type, data_type, &data);
    }
    return 1;
}
