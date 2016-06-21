#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "queue.h"


int open_queue(struct queue_state *state) {
    struct mq_attr ma;
    ma.mq_flags = 0;
    ma.mq_maxmsg = 10;
    ma.mq_msgsize = 256;
    ma.mq_curmsgs = 0;
    mq_unlink(QUEUE_NAME);
    state->queue_id = mq_open(QUEUE_NAME, O_RDWR | O_CREAT, S_IWUSR | S_IRUSR, &ma);
    if (state->queue_id < 0) {
        perror("mq_open");
    }
    return 0;
}


int close_queue(struct queue_state *state) {
    mq_close(state->queue_id);
    mq_unlink(QUEUE_NAME);
    return 0;
}


int check_queue(struct queue_state *state) {
    char msgbuf [1024];
    if (state->queue_id < 0) {
        return 0;
    }
    ssize_t recv = mq_receive(state->queue_id, msgbuf, 1024, NULL);
    if (recv > 0) {
        printf("recv %d: %s\n", recv, msgbuf);
    }
    else {
        perror("mq_receive");
    }
}
