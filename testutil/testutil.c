#include <stdio.h>
#include <unistd.h>

#include <libutil/endpoint.h>
#include <libutil/socket.h>


void *on_accept(struct ep_source *s) {
    printf("accept notify\n");
    return NULL;
}


void *on_read(struct ep_source *s) {
    printf("read notify\n");
    return NULL;
}


void ep_test() {
    struct ep_table tb;
    ep_table_init(&tb, "");

    // create a listener
    struct ep_address *addr = ep_new_addr(&tb);
    ep_set_local(addr, "testname");
    struct ep_source *src = ep_new_src(&tb, addr->epid);
    ep_set_src(src);
    ep_activate_acceptor(&tb, addr->epid, on_accept, on_read);

    // create a connector
    addr = ep_new_addr(&tb);
    ep_set_local(addr, "testname");
    src = ep_new_src(&tb, addr->epid);
    ep_set_src(src);
    ep_activate_connector(addr, on_read);

    // wait until threads complete
    ep_table_join(&tb);
    ep_table_free(&tb);
}


int main(int argc, char *argv[]) {
    ep_test();
}
