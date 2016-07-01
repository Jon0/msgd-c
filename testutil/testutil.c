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


void ep_map_test() {
    struct ep_table tb;
    ep_table_init(&tb, "");
    printf("array size: %d\n", tb.avail);
    struct ep_address *addr1 = ep_new_addr(&tb);
    int new_id = addr1->epid;
    printf("id: %d\n", new_id);

    struct ep_address *addr2 = ep_table_addr(&tb, new_id);
    printf("addr: %x, %x\n", addr1, addr2);

    // test src creation
    struct ep_source *s = ep_table_src(&tb, new_id);
    printf("src: %x\n", s);
    ep_new_src(&tb, new_id);
    s = ep_table_src(&tb, new_id);
    printf("src: %x\n", s);

    // test dest creation
    struct ep_dest *d = ep_table_dest(&tb, new_id);
    printf("dest: %x\n", d);
    ep_new_dest(&tb, new_id);
    d = ep_table_dest(&tb, new_id);
    printf("dest: %x\n", d);

    ep_table_free(&tb);
}


void ep_test() {
    struct ep_table tb;
    ep_table_init(&tb, "");

    // create a listener
    struct ep_address *addr = ep_new_addr(&tb);
    ep_set_local(addr, "testname");
    ep_add_pipe_endpoints(&tb, addr->epid);
    ep_activate_acceptor(&tb, addr->epid, on_accept, on_read);

    // create a connector
    addr = ep_new_addr(&tb);
    ep_set_local(addr, "testname");
    ep_add_pipe_endpoints(&tb, addr->epid);
    ep_activate_connector(addr, on_read);

    // wait until threads complete
    ep_table_join(&tb);
    ep_table_free(&tb);
}


int main(int argc, char *argv[]) {
    ep_map_test();
    ep_test();
}
