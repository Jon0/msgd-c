#include <stdio.h>
#include <string.h>

#include "ipc.h"


void mdu_ipc_server(struct ipc_channel *c, const char *name) {
    struct mq_attr ma;
    ma.mq_flags = 0;
    ma.mq_maxmsg = 10;
    ma.mq_msgsize = 256;
    ma.mq_curmsgs = 0;
    mq_unlink(QUEUE_NAME);
    c->queue_id = mq_open(QUEUE_NAME, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, &ma);
    if (c->queue_id < 0) {
        perror("mq_open");
    }
}


void mdu_ipc_client(struct ipc_channel *c, const char *name) {
    c->queue_id = mq_open(QUEUE_NAME, O_RDWR);
    if (c->queue_id < 0) {
        perror("mq_open");
    }
    else {
        strcpy(c->queue_name, name);
        mq_send(c->queue_id, c->queue_name, 256, 0);
    }
}


void mdu_ipc_close(struct ipc_channel *c) {
    if (c->queue_id != -1) {
        int err = mq_close(c->queue_id);
        if (err) {
            perror("mq_close");
        }
    }
}


ssize_t mdu_ipc_send(struct ipc_channel *c, char *buf, size_t count) {
    if (c->queue_id < 0) {
        return 0;
    }
    ssize_t sent = mq_send(c->queue_id, buf, count, 0);
    if (sent < 0) {
        perror("mq_send");
    }
    return sent;
}


ssize_t mdu_ipc_recv(struct ipc_channel *c, char *buf, size_t count) {
    if (c->queue_id < 0) {
        return 0;
    }
    ssize_t recv = mq_receive(c->queue_id, buf, count, NULL);
    if (recv < 0) {
        perror("mq_receive");
    }
    return recv;
}
