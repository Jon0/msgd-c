#include <stdio.h>
#include <unistd.h>

#include <libutil/endpoint.h>
#include <libutil/socket.h>


void *recvnote(void *p) {
    return NULL;
}


void ep_test() {
    struct ep_table tb;
    ep_table_init(&tb, "");
    ep_create_acceptor(tb.src, recvnote);
    ep_table_free(&tb);
}


int main(int argc, char *argv[]) {
    ep_test();
}
