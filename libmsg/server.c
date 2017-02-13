#include <stdio.h>
#include <string.h>

#include "server.h"


void msg_server_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_server *serv = p;
    struct msgs_socket newsocket;
    char sockname [128];

    // which acceptor was updated?
    if (e->id == serv->local_acc_id) {
        while (msgs_accept_socket(&serv->local_acc, &newsocket)) {
            msgs_address_print(sockname, &newsocket.addr);
            printf("[server] accept: %s from %d (local)\n", sockname, e->id);
            int newid = msg_hostlist_init_connection(&serv->hostlist, &serv->emap, &newsocket);
            msgs_poll_socket(&serv->tb, &newsocket, newid);
        }
    }
    else if (e->id == serv->remote_acc_id) {
        while (msgs_accept_socket(&serv->remote_acc, &newsocket)) {
            msgs_address_print(sockname, &newsocket.addr);
            printf("[server] accept: %s from %d (remote)\n", sockname, e->id);
            int newid = msg_hostlist_init_connection(&serv->hostlist, &serv->emap, &newsocket);
            msgs_poll_socket(&serv->tb, &newsocket, newid);
        }
    }
    else {
        printf("[server] accept: %d (unknown)\n", e->id);
    }
}


void msg_server_recv_event(void *p, struct msgu_recv_event *e) {
    struct msg_server *serv = p;
    msg_hostlist_connection_notify(&serv->hostlist, e->id);
}


void msg_server_mount_event(void *serv, struct msgu_mount_event *e) {
    msg_server_notify_mount(serv, e);
}


static struct msgu_handlers msg_server_handlers = {
    .connect_event    = msg_server_connect_event,
    .recv_event       = msg_server_recv_event,
    .mount_event      = msg_server_mount_event,
};


int msg_server_message_recv(struct msg_connection *conn, struct msgu_message *msg, void *serv) {
    return msg_server_recv(serv, conn, msg);
}


int msg_server_mount_callback(void *p, struct msg_connection *conn, const struct msgu_message *msg) {
    struct msg_server *serv = p;
    switch (msg->event_type) {
    case msgtype_return_node_handle:
        msgs_fuse_notify(serv->fuse);
        break;
    case msgtype_return_node_content:
        msgu_string_copy(&serv->fuse->reply.data, &msg->buf.data.node_write.data);
        msgs_fuse_notify(serv->fuse);
        break;
    }
    return 0;
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


void msg_server_mount_pass(struct msg_server *serv, struct msgu_mount_event *e) {
    struct msgu_mount_address ma;
    const char *remain;
    int len = msgu_mount_address_path(&ma, &remain, e->path);
    if (len != 2) {
        return;
    }

    struct msgu_mount_point *mp = msgu_mount_get(&serv->mounts, &ma);
    if (mp == NULL) {
        return;
    }

    // send request
    msgs_mutex_t *conn_mutex;
    struct msg_connection *conn = msg_hostlist_use_host(&serv->hostlist, &conn_mutex, &ma.host_name);
    if (conn) {
        union msgu_any_msg data;
        int msg_type;
        int msg_data;
        switch (e->event_type) {
        case msgu_mount_event_open:
            msg_type = msgtype_file_open;
            msg_data = msgdata_share_file;
            msgu_string_copy(&data.share_file.share_name, &ma.share_name);
            break;
        case msgu_mount_event_read:
            msg_type = msgtype_file_stream_read;
            msg_data = msgdata_node_read;
            data.node_read.node_handle = mp->open_handle;
            data.node_read.count = e->size;
            break;
        case msgu_mount_event_write:
            msg_type = msgtype_file_stream_write;
            msg_data = msgdata_node_write;
            data.node_write.node_handle = mp->open_handle;
            msgu_string_from_buffer(&data.node_write.data, e->data, e->size);
            break;
        }

        // create notifier to accept reply
        int msg_id = serv->msg_id++;
        msg_notify_map_add(&serv->notify, msg_id, msg_server_mount_callback, serv);
        msg_connection_send_message(conn, msg_id, msg_type, msg_data, &data);
        msgs_mutex_unlock(conn_mutex);
    }
}


void msg_server_notify_mount(struct msg_server *serv, struct msgu_mount_event *e) {
    printf("server fuse event: %d, %s\n", e->event_type, e->path);
    switch (e->event_type) {
    case msgu_mount_event_attr:
        serv->fuse->reply.exists = msgu_mount_attr(&serv->mounts, e->path, &serv->fuse->reply.node);
        msgs_fuse_notify(serv->fuse);
        break;
    case msgu_mount_event_list:
        serv->fuse->reply.exists = msgu_mount_list(&serv->mounts, e->path, &serv->fuse->reply.nodes);
        msgs_fuse_notify(serv->fuse);
        break;
    case msgu_mount_event_open:
    case msgu_mount_event_read:
    case msgu_mount_event_write:
        msg_server_mount_pass(serv, e);
        break;
    }
}


void msg_server_print_state(struct msg_server *serv) {
    printf("[server state] ");
    msg_hostlist_print(&serv->hostlist);
    msgu_share_debug(&serv->shares);
}


void msg_server_init(struct msg_server *s, const char *sockpath) {
    msgs_set_signals();
    msgu_event_map_init(&s->emap, &msg_server_handlers, s);
    msg_notify_map_init(&s->notify, 32);
    msg_host_list_init(&s->hostlist, 32, msg_server_message_recv, s);
    msgu_share_set_init(&s->shares, &file_ops);
    msgu_mount_map_init(&s->mounts, 32);
    msgs_file_cache_init(&s->cache, &s->shares);
    msgs_table_init(&s->tb, &s->emap);
    msgs_host_init_self(&s->self);
    s->msg_id = 1;


    // create a local acceptor
    struct msgu_address local_addr;
    ep_unlink("msgd-ipc");
    ep_local(&local_addr, "msgd-ipc");
    msgs_open_acceptor(&s->local_acc, &local_addr);
    s->local_acc_id = msgu_add_connect_handler(&s->emap);
    msgs_poll_acceptor(&s->tb, &s->local_acc, s->local_acc_id);


    // create a remote acceptor
    struct msgu_address raddr;
    ep_listen_remote(&raddr, 2204);
    msgs_open_acceptor(&s->remote_acc, &raddr);
    s->remote_acc_id = msgu_add_connect_handler(&s->emap);
    msgs_poll_acceptor(&s->tb, &s->remote_acc, s->remote_acc_id);


    // create loopback connection
    msg_server_connect(s, "127.0.0.1");
    s->fuse = msgs_fuse_static_start(&s->emap, "fusemount");
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
    int cid = msg_hostlist_init_connection(&serv->hostlist, &serv->emap, &socket);
    struct msg_connection *conn = msg_hostlist_use_id(&serv->hostlist, &lock, cid);
    if (conn) {
        msg_connection_send_message(conn, serv->msg_id++, msgtype_init_remote, msgdata_init_remote, (union msgu_any_msg *) &init_msg);
        msgs_mutex_unlock(lock);
        msgs_poll_socket(&serv->tb, &socket, cid);
    }
    else {
        printf("failed to add connection\n");
        return -1;
    }
    return cid;
}


void msg_server_run(struct msg_server *serv) {

    // waits until threads complete
    msgs_table_start(&serv->tb, 4);
    msgs_table_free(&serv->tb);
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
