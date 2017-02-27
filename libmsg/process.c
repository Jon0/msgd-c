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


int msg_server_recv_mount(struct msg_process *proc, struct msgu_mount_point *mnt, struct msgu_message *msg) {
    // TODO
}


void msg_main_init(struct msg_process *proc, const char *sockpath) {
    msgs_set_signals();
    msg_system_init(&proc->external, &msg_server_handlers, proc);
    msg_server_init(&proc->state, &proc->external);

    // create a local acceptor
    struct msgu_address local_addr;
    ep_unlink("msgd-ipc");
    ep_local(&local_addr, "msgd-ipc");
    proc->local_acc_id = msg_system_create_acceptor(&proc->external, &local_addr, &proc->local_acc);

    // create a remote acceptor
    struct msgu_address raddr;
    ep_listen_remote(&raddr, 2204);
    proc->remote_acc_id = msg_system_create_acceptor(&proc->external, &raddr, &proc->remote_acc);

    // create loopback connection
    msg_server_connect(&proc->state, "127.0.0.1");
}


void msg_main_run(struct msg_process *proc) {

    // waits until threads complete
    msgs_table_start(&proc->external.tb, 4);
    msgs_table_free(&proc->external.tb);
}
