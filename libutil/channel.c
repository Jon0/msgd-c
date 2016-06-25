#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "channel.h"


void ch_array_init(struct ch_read_array *arr, size_t chanmax, size_t epmax) {
    size_t chanbytes = sizeof(struct channel) * chanmax;
    size_t epbytes = sizeof(struct endpoint) * epmax;
    size_t total = chanbytes + epbytes;

    // allocate as one large block
    printf("allocate array (%d bytes)\n", total);
    char *mem = malloc(total);
    arr->chanmem = (struct channel *) mem;
    arr->epmem = (struct endpoint *) &mem[chanbytes];
    arr->ccount = 0;
    arr->cavail = chanmax;
    arr->ecount = 0;
    arr->eavail = epmax;
}


void ch_array_free(struct ch_read_array *arr) {
    free(arr->chanmem);
}


void ch_array_insert(struct ch_read_array *arr, struct channel *c) {
    size_t cspace = arr->cavail - arr->ccount;
    size_t espace = arr->eavail - arr->ecount;
    if (cspace < 1 || espace < c->count) {
        printf("array buffer full\n");
    }
    else {
        struct channel *back = &arr->chanmem[arr->ccount];
        back->count = c->count;
        back->in = c->in;
        back->out = &arr->epmem[arr->ecount];

        // copy endpoints
        for (int i = 0; i < c->count; ++i) {
            arr->epmem[arr->ecount + i] = c->out[i];
        }
        arr->ecount += c->count;
        ++arr->ccount;
    }
}


void ch_array_remove(struct ch_read_array *arr, size_t index) {
    --arr->ccount;
    for (int i = index; i < arr->ccount; ++i) {
        arr->chanmem[i] = arr->chanmem[i + 1];
    }

    // reformat endpoint data
    size_t eindex = 0;
    for (int i = 0; i < arr->ccount; ++i) {
        struct channel *chan = &arr->chanmem[i];
        chan->out = &arr->epmem[eindex];
        for (int j = 0; j < chan->count; ++j) {
            arr->epmem[eindex++] = chan->out[j];
        }
    }
    arr->ecount = eindex;
}


void ch_queue_init(struct ch_write_queue *queue, size_t queuemax, size_t taskmax) {
    size_t queuebytes = sizeof(struct write_stat) * queuemax;
    size_t taskbytes = sizeof(struct write_task) * taskmax;
    size_t total = queuebytes + taskbytes;

    // allocate as one large block
    printf("allocate queue (%d bytes)\n", total);
    char *mem = malloc(total);
    queue->queuemem = (struct write_stat *) mem;
    queue->taskmem = (struct write_task *) &mem[queuebytes];
    queue->qfront = 0;
    queue->qcount = 0;
    queue->qavail = queuemax;
    queue->tfront = 0;
    queue->tcount = 0;
    queue->tavail = taskmax;
}


void ch_queue_free(struct ch_write_queue *queue) {
    free(queue->queuemem);
}


void ch_queue_safe_insert(struct ch_write_queue *queue, struct channel *c, size_t cc) {
    for (int i = 0; i < cc; ++i) {
        size_t qspace = queue->qavail - queue->qcount;
        size_t tspace = queue->tavail - queue->tcount;
        if (qspace < 1 || tspace < c[i].count) {
            printf("queue buffer full\n");
        }
        else {
            ch_queue_insert(queue, &c[i]);
        }
    }
}


void ch_queue_insert(struct ch_write_queue *queue, struct channel *c) {
    size_t qback = (queue->qfront + queue->qcount) % queue->qavail;
    struct write_stat *back = &queue->queuemem[qback];
    back->bufsize = ep_read(&c->in, back->buffer, BUFFER_SIZE);
    if (back->bufsize > 0) {
        size_t tback = (queue->tfront + queue->tcount) % queue->tavail;
        back->tasks = &queue->taskmem[tback];
        back->tcount = c->count;
        for (int i = 0; i < back->tcount; ++i) {
            struct write_task *tb = &queue->taskmem[(tback + i) % queue->tavail];
            tb->ep = c->out[i];
            tb->remain = back->bufsize;
        }
        queue->tcount += c->count;
        ++queue->qcount;
    }
}


void ch_queue_pop(struct ch_write_queue *queue) {
    printf("pop front block\n");
    struct write_stat *front = &queue->queuemem[queue->qfront];
    queue->tfront += front->tcount;
    queue->tcount -= front->tcount;
    queue->qfront += 1;
    queue->qcount -= 1;
}


int ch_queue_write(struct write_stat *front) {
    int incomplete = 0;
    for (int i = 0; i < front->tcount; ++i) {
        struct write_task *task = &front->tasks[i];
        if (task->remain > 0) {
            size_t st = front->bufsize - task->remain;
            ssize_t copied = ep_write(&task->ep, &front->buffer[st], task->remain);
            if (copied > 0) {
                task->remain -= copied;
            }
            else {
                printf("error on write\n");
            }
            if (task->remain > 0) {
                incomplete++;
            }
        }
    }
    return incomplete;
}


void ch_update(struct ch_read_array *arr, struct ch_write_queue *queue) {
    ch_queue_safe_insert(queue, arr->chanmem, arr->ccount);

    struct write_stat *front = &queue->queuemem[queue->qfront];
    if (ch_queue_write(front) == 0) {
        ch_queue_pop(queue);
    }
}


ssize_t ep_read(struct endpoint *in, char *buffer, size_t count) {
    printf("ep_read\n");
    switch (in->mode) {
    case local:
        return read(in->id, buffer, count);
    case module:
        return read(in->id, buffer, count);
    default:
        return 0;
    }
}


ssize_t ep_write(struct endpoint *out, char *buffer, size_t count) {
    printf("ep_write\n");
    switch (out->mode) {
    case local:
        return write(out->id, buffer, count);
    default:
        return 0;
    }
}
