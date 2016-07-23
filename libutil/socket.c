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


void ep_set_remote(struct ep_address *a, short portnum) {
    struct sockaddr_in *addr = (struct sockaddr_in *) &a->addr;
    bzero(addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(portnum);
    a->addrlen = sizeof(struct sockaddr_in);
}


void ep_new_endpoints(struct ep_table *t, int epid, int type, ep_fwd_t r) {

    // what if src or dest already exist?
    struct ep_source *s = ep_new_src(t, epid);
    struct ep_dest *d = ep_new_dest(t, epid);

    // create a unix socket
    s->fd = socket(type, SOCK_STREAM, 0);
    s->func = r;
    if (s->fd < 0) {
        perror("socket");
        return;
    }
    d->fd = s->fd;
    ep_enable_src(t, s);
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


void ep_activate_connector(struct ep_address *a) {
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


int ep_on_accept_local(struct ep_table *t, int src, int out) {
    struct ep_address *this_addr = ep_table_addr(t, src);
    struct ep_source *this_src = this_addr->src;
    struct sockaddr_un addr;
    socklen_t len = sizeof(addr);
    int fd = accept(this_src->fd, (struct sockaddr *) &addr, &len);
    if (fd < 0) {
        perror("accept");
        return 0;
    }

    // make a new table entry
    struct ep_handler *h = ep_new_hdl(t, this_src->read);
    struct ep_address *a = ep_new_addr(t, h->epid);
    memcpy(a->addr, &addr, len);
    a->addrlen = len;
    ep_accept_endpoints(t, h->epid, fd);
    return 0;
}

int ep_on_accept_net(struct ep_table *t, int src, int out) {
    struct ep_address *this_addr = ep_table_addr(t, src);
    struct ep_source *this_src = this_addr->src;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int fd = accept(this_src->fd, (struct sockaddr *) &addr, &len);
    if (fd < 0) {
        perror("accept");
        return 0;
    }

    // make a new table entry
    struct ep_handler *h = ep_new_hdl(t, this_src->read);
    struct ep_address *a = ep_new_addr(t, h->epid);
    memcpy(a->addr, &addr, len);
    a->addrlen = len;
    ep_accept_endpoints(t, h->epid, fd);
    printf("created new epid %d\n", h->epid);
    return 0;
}


int ep_on_recv(struct ep_table *t, int src, int out) {
    printf("ep_on_recv\n");
    return 0;
}


void ep_accept_endpoints(struct ep_table *t, int epid, int fd) {
    // initialise source and dest
    struct ep_source *s = ep_new_src(t, epid);
    s->fd = fd;
    s->func = ep_on_recv;
    struct ep_dest *dt = ep_new_dest(t, epid);
    dt->fd = fd;
    ep_enable_src(t, s);
}


void ep_tcp_acceptor(struct ep_table *t, struct ep_handler *h, ep_callback_t c) {

    // add to endpoint table
    struct ep_address *addr = ep_new_addr(t, h->epid);
    ep_set_remote(addr, 2240);
    ep_new_endpoints(t, h->epid, AF_INET, ep_on_accept_net);
    ep_activate_acceptor(addr);
}
