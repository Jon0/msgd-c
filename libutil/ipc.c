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


void mdu_ipc_send(struct ipc_channel *c) {
    mq_send(c->queue_id, c->queue_name, 256, 0);
}


void mdu_ipc_recv(struct ipc_channel *c) {
    char msgbuf [1024];
    if (c->queue_id < 0) {
        return;
    }
    ssize_t recv = mq_receive(c->queue_id, msgbuf, 1024, NULL);
    if (recv > 0) {
        printf("recv %d: %s\n", recv, msgbuf);
    }
    else {
        perror("mq_receive");
    }
}
