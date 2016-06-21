#ifndef QUEUE_H
#define QUEUE_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#define QUEUE_NAME "/local_msgd"


struct queue_state {
    mqd_t queue_id;
};


int open_queue(struct queue_state *state);
int close_queue(struct queue_state *state);
int check_queue(struct queue_state *state);

#endif
