#include <stdio.h>
#include <string.h>

#include "msgd.h"
#include "queue.h"


void md_init_proc(struct node_state *ns, const char *name, int mode) {
    ns->queue_id = mq_open("/local_msgd", O_RDWR);
    if (ns->queue_id < 0) {
        perror("mq_open");
    }
    else {
        strcpy(ns->queue_name, name);
        mq_send(ns->queue_id, ns->queue_name, 256, 0);
    }
}


void md_free_proc(struct node_state *ns) {
    if (ns->queue_id != -1) {
        int err = mq_close(ns->queue_id);
        if (err) {
            perror("mq_close");
        }
    }
}


void md_update(struct node_state *ns) {
    mq_send(ns->queue_id, ns->queue_name, 256, 0);
}
