#include <stdio.h>
#include <string.h>

#include "host.h"


int msg_host_init(struct msgu_host *h, const char *addr, const char *name) {
    h->interface = 0;
    h->addr.len = strlen(addr);
    strcpy(h->addr.data, addr);
    strcpy(h->hostname, name);
    return 0;
}


int msg_host_list_init(struct msg_host_list *h) {
    h->host_count = 0;
    h->host_max = 0;
    h->ptr = NULL;
}


int msg_host_list_add(struct msg_host_list *h, const char *addr, const char *name) {
    if (h->host_count == h->host_max) {
        return -1;
    }
    int index = h->host_count++;
    msg_host_init(&h->ptr[index], addr, name);
    return index;
}


struct msgu_host *msg_host_match(struct msg_host_list *h, const char *hostname) {
    for (int i = 0; i < h->host_count; ++i) {
        if (strcmp(hostname, h->ptr[i].hostname) == 0) {
            return &h->ptr[i];
        }
    }
    return NULL;
}
