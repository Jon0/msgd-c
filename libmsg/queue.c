#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "queue.h"

int create_queue(struct queue_state *state) {
    state->queue_id = mq_open("local_msgd", O_RDWR | O_CREAT);
    return 0;
}


int close_queue(struct queue_state *state) {
    mq_close(state->queue_id);
    return 0;
}


int check_queue(struct queue_state *state) {
    char msgbuf [1024];
    ssize_t recv = mq_receive(state->queue_id, msgbuf, 1024, 0);
    if (recv) {
        printf("recv %d: %s\n", recv, msgbuf);
    }
}
