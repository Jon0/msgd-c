#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

#include "queue.h"

int init_queue(struct queue_state *state) {
    state->queue_id = mq_open("local_msgd", O_RDWR);
    return 0;
}
