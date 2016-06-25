#include <stdio.h>

#include <libutil/channel.h>


ssize_t test_read(int id, char *buffer, size_t count) {
    return count;
}


ssize_t test_write(int id, char *buffer, size_t count) {
    return count;
}


void print_array_state(struct ch_read_array *r) {
    printf("channel count: %d\n", r->ccount);
    printf("endpoint count: %d\n", r->ecount);
}

void chantest() {
    struct ch_read_array r;
    struct ch_write_queue w;
    printf("test channels\n");
    ch_array_init(&r, 256, 1024);
    ch_queue_init(&w, 256, 1024);

    // add a channel
    struct endpoint ep;
    ep.id = 1;
    struct channel c;
    c.count = 1;
    c.out = &ep;
    c.in.id = 0;
    c.in.fn = test_read;
    ch_array_insert(&r, &c);
    print_array_state(&r);

    // test update
    ch_update(&r, &w);


    // remove first channel
    ch_array_remove(&r, 0);
    print_array_state(&r);


    ch_queue_free(&w);
    ch_array_free(&r);
}


int main(int argc, char *argv[]) {
    chantest();
}
