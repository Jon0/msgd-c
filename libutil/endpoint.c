#include <string.h>
#include <unistd.h>

#include "endpoint.h"


void ep_table_init(struct ep_table *t, char *path) {
    t->src = malloc(sizeof(struct ep_source) * 1024);
    t->avail = 1024;
    t->src_count = 0;
    t->next_id = 1;
    strcpy(t->path, path);
}


void ep_table_free(struct ep_table *t) {
    t->avail = 0;
    t->src_count = 0;
    free(t->src);
}


void ep_table_join(struct ep_table *t) {
    // how many threads are running?
    pthread_join(t->src[0].thread, NULL);
}


size_t ep_table_hash(struct ep_table *t, int epid) {
    return epid % t->avail;
}


struct ep_address *ep_new_addr(struct ep_table *t) {
    size_t id = t->next_id++;
    struct ep_address *a = &t->addr[id];
    a->epid = id;
    a->addrlen = 0;
    return a;
}


struct ep_source *ep_new_src(struct ep_table *t, int epid) {

}


struct ep_dest *ep_new_dest(struct ep_table *t, int epid) {

}


struct ep_address *ep_table_addr(struct ep_table *t, int epid) {
    return NULL;
}


struct ep_source *ep_table_src(struct ep_table *t, int epid) {
    struct ep_source *back = &t->src[t->src_count];
}


struct ep_dest *ep_table_dest(struct ep_table *t, int epid) {
    return NULL;
}
