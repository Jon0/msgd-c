#include <string.h>

#include "endpoint.h"


void ep_table_init(struct ep_table *t, char *path) {
    t->src = malloc(sizeof(struct ep_source) * 1024);
    t->src_avail = 1024;
    t->src_count = 0;
    strcpy(t->path, path);
}


void ep_table_free(struct ep_table *t) {
    t->src_avail = 0;
    t->src_count = 0;
    free(t->src);
}


void ep_table_join(struct ep_table *t) {
    // how many threads are running?
    pthread_join(t->src[0].thread, NULL);
}

struct ep_source *ep_table_add(struct ep_table *t) {
    // fix running past end of array
    struct ep_source *back = &t->src[t->src_count];
    ++t->src_count;
    return back;
}
