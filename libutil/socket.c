#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "socket.h"


void ep_unlink(const char *address) {
    unlink(address);
}


void ep_set_local(struct ep_address *a, const char *address) {

    // intialise address struct with address name
    struct sockaddr_un *saun = (struct sockaddr_un *) &a->addr;
    saun->sun_family = AF_UNIX;
    strcpy(saun->sun_path, address);
    a->addrlen = sizeof(saun->sun_family) + strlen(saun->sun_path);
}


void ep_add_pipe_endpoints(struct ep_table *t, int epid) {

    // what if src or dest already exist?
    struct ep_source *s = ep_new_src(t, epid);
    struct ep_dest *d = ep_new_dest(t, epid);

    // create a unix socket
    s->fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s->fd < 0) {
        perror("socket");
        return;
    }
    d->fd = s->fd;
}


void ep_on_accept(struct ep_table *t, struct ep_handler *h) {
    struct ep_address *this_addr = h->id.addr;
    struct ep_source *this_src = this_addr->src;
    struct sockaddr_un addr;
    socklen_t len = sizeof(addr);


    // wait for events on the socket
    int fd = accept(this_src->fd, (struct sockaddr *) &addr, &len);
    printf("accept %d\n", fd);

    // make a new table entry
    struct ep_address *a = ep_new_addr(t);
    memcpy(a->addr, &addr, len);
    a->addrlen = len;

    // initialise source and dest
    struct ep_source *s = ep_new_src(t, a->epid);
    s->fd = fd;
    struct ep_dest *d = ep_new_dest(t, a->epid);
    d->fd = fd;

}


void ep_activate_acceptor(struct ep_address *a) {

    // the acceptor requires an initialised addr and src
    int err;
    if (a && a->src) {
        printf("creating socket acceptor\n");

        // bind and listen on the socket
        err = bind(a->src->fd, (struct sockaddr *) &a->addr, a->addrlen);
        if (err < 0) {
            perror("bind");
            return;
        }
        listen(a->src->fd, 5);
    }
    else {
        printf("failed to start acceptor\n");
    }
}


void ep_activate_connector(struct ep_address *a, notify_fn_t rf, void *obj) {
    printf("creating socket connector\n");

    // the connector requires an initialised addr and src
    if (a && a->src) {

        // connect to address
        int err = connect(a->src->fd, (struct sockaddr *) &a->addr, a->addrlen);
        if (err == -1) {
            perror("connect");
            return;
        }
    }
}
