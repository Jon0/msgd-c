#ifndef QUEUE_H
#define QUEUE_H

#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>


struct queue_state {
    mqd_t queue_id;
};


int init_queue(struct queue_state *state);


#endif
