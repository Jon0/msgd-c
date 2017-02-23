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
            int newid = msg_hostlist_init_connection(&proc->state.hostlist, &proc->emap, &newsocket);
            msgs_poll_socket(&proc->tb, &newsocket, newid);
        }
    }
    else if (e->id == proc->remote_acc_id) {
        while (msgs_accept_socket(&proc->remote_acc, &newsocket)) {
            msgs_address_print(sockname, &newsocket.addr);
            printf("[server] accept: %s from %d (remote)\n", sockname, e->id);
            int newid = msg_hostlist_init_connection(&proc->state.hostlist, &proc->emap, &newsocket);
            msgs_poll_socket(&proc->tb, &newsocket, newid);
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


void msg_main_init(struct msg_process *proc, const char *sockpath) {
    msgs_set_signals();
    msg_server_init(&proc->state);


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
