#ifndef CHANNEL_H
#define CHANNEL_H

#include <stdint.h>

#define BUFFER_SIZE 4096
#define QUEUE_DEFAULT 16


typedef ssize_t (*transfer)(int, char *, size_t);


// endpoint is local, module or network
// local endpoints are file descriptors
enum ep_mode {
    local,
    remote,
    module
};


struct endpoint {
    enum ep_mode   mode;
    int64_t        id;
    transfer       fn;
};


struct channel {
    size_t            count;
    struct endpoint   in;
    struct endpoint  *out;
};


/*
 * queued write to a file descriptor
 */
struct write_task {
    struct endpoint   ep;
    size_t            remain;
};


struct write_stat {
    struct write_task *tasks;
    size_t tcount;
    ssize_t bufsize;
    char buffer [BUFFER_SIZE];
};


/*
 * arrays start at pointers
 */
struct ch_read_array {
    struct channel     *chanmem;
    struct endpoint    *epmem;
    size_t ccount;
    size_t cavail;
    size_t ecount;
    size_t eavail;
};


/*
 * memory allocations for queue
 */
struct ch_write_queue {
    struct write_stat  *queuemem;
    struct write_task  *taskmem;
    size_t qfront;
    size_t qcount;
    size_t qavail;
    size_t tfront;
    size_t tcount;
    size_t tavail;
};


/*
 * basic io functions
 */
ssize_t ep_read(struct endpoint *in, char *buffer, size_t count);
ssize_t ep_write(struct endpoint *out, char *buffer, size_t count);


void ch_array_init(struct ch_read_array *arr, size_t chanmax, size_t epmax);
void ch_array_free(struct ch_read_array *arr);
void ch_array_insert(struct ch_read_array *arr, struct channel *c);
void ch_array_remove(struct ch_read_array *arr, size_t index);


/*
 * allocate queue memory
 */
void ch_queue_init(struct ch_write_queue *queue, size_t queuemax, size_t taskmax);
void ch_queue_free(struct ch_write_queue *queue);


/*
 * check queue has available memory, and try poll before read
 */
void ch_queue_safe_insert(struct ch_write_queue *queue, struct channel *c, size_t cc);


/*
 * poll and insert from input channel
 */
void ch_queue_insert(struct ch_write_queue *queue, struct channel *c);
void ch_queue_pop(struct ch_write_queue *queue);


/*
 * try write item at the front of the queue
 * return number of remaining tasks
 */
int ch_queue_write(struct write_stat *queue);


/*
 * queue new items
 */
void ch_update(struct ch_read_array *arr, struct ch_write_queue *queue);

#endif
