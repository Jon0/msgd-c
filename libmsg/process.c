#include <stdio.h>

#include "process.h"


void msg_server_connect_event(void *p, struct msgu_connect_event *e) {
    struct msg_process *proc = p;
    struct msgs_socket newsocket;
    char sockname [128];

    // which acceptor was updated?
    if (e->id == proc->local_acc_id) {
        while (msgs_accept_socket(&proc->local_acc, &newsocket)) {
            msgs_address_print(sockname, &newsocket.addr);
            printf("[server] accept: %s from %d (local)\n", sockname, e->id);
            msg_server_accept(&proc->state, &newsocket);
        }
    }
    else if (e->id == proc->remote_acc_id) {
        while (msgs_accept_socket(&proc->remote_acc, &newsocket)) {
            msgs_address_print(sockname, &newsocket.addr);
            printf("[server] accept: %s from %d (remote)\n", sockname, e->id);
            msg_server_accept(&proc->state, &newsocket);
        }
    }
    else {
        printf("[server] accept: %d (unknown)\n", e->id);
    }
}


void msg_server_recv_event(void *p, struct msgu_recv_event *e) {
    struct msg_process *proc = p;
    msg_hostlist_connection_notify(&proc->state.hostlist, e->id);
}


void msg_server_mount_event(void *p, struct msgu_mount_event *e) {
    struct msg_process *proc = p;
    msg_server_notify_mount(&proc->state, e);
}


static struct msgu_handlers msg_server_handlers = {
    .connect_event    = msg_server_connect_event,
    .recv_event       = msg_server_recv_event,
    .mount_event      = msg_server_mount_event,
};


int msg_server_mount_callback(struct msg_process *proc, void *intf, const struct msgu_message *msg) {
    // called from notify table
    struct msg_connection *conn = intf;
    switch (msg->event_type) {
    case msgtype_return_node_handle:
        msgs_fuse_notify(proc->fuse);
        break;
    case msgtype_return_node_content:
        msgu_string_copy(&proc->fuse->reply.data, &msg->buf.data.node_write.data);
        msgs_fuse_notify(proc->fuse);
        break;
    }
    return 0;
}


void msg_server_mount_pass(struct msg_process *proc, struct msgu_mount_event *e) {
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
        msg_notify_map_add(&serv->notify, msg_id, msg_server_mount_callback, proc);
        msg_connection_send_message(conn, msg_id, msg_type, msg_data, &data);
        msgs_mutex_unlock(conn_mutex);
    }
}


void msg_server_notify_mount(struct msg_process *proc, struct msgu_mount_event *e) {
    printf("server fuse event: %d, %s\n", e->event_type, e->path);

    // TODO convert mount events to messages
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
        msg_server_mount_pass(proc, e);
        break;
    }
}


int msg_server_recv_mount(struct msg_process *proc, struct msgu_mount_point *mnt, struct msgu_message *msg) {
    // TODO
}


void msg_main_init(struct msg_process *proc, const char *sockpath) {
    msgs_set_signals();
    msgu_event_map_init(&proc->emap, &msg_server_handlers, proc);
    msgs_table_init(&proc->tb, &proc->emap);
    msg_server_init(&proc->state, &proc->tb);


    // create a local acceptor
    struct msgu_address local_addr;
    ep_unlink("msgd-ipc");
    ep_local(&local_addr, "msgd-ipc");
    msgs_open_acceptor(&proc->local_acc, &local_addr);
    proc->local_acc_id = msgu_add_connect_handler(&proc->emap);
    msgs_poll_acceptor(&proc->tb, &proc->local_acc, proc->local_acc_id);


    // create a remote acceptor
    struct msgu_address raddr;
    ep_listen_remote(&raddr, 2204);
    msgs_open_acceptor(&proc->remote_acc, &raddr);
    proc->remote_acc_id = msgu_add_connect_handler(&proc->emap);
    msgs_poll_acceptor(&proc->tb, &proc->remote_acc, proc->remote_acc_id);


    // create loopback connection
    msg_server_connect(&proc->state, "127.0.0.1");
    proc->fuse = msgs_fuse_static_start(&proc->emap, "fusemount");
}


void msg_main_run(struct msg_process *proc) {

    // waits until threads complete
    msgs_table_start(&proc->tb, 4);
    msgs_table_free(&proc->tb);
}
