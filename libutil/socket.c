#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "socket.h"


void ep_unlink(char *address) {
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
    s->ksrc.fd = socket(AF_UNIX, SOCK_STREAM, 0);
    s->ksrc.events = POLLIN;
    if (s->ksrc.fd < 0) {
        perror("socket");
        return;
    }
    d->fd = s->ksrc.fd;
}


void *ep_thread_accept(void *p) {
    struct ep_accept_data *ld = (struct ep_accept_data *) p;
    struct ep_address *this_addr = ld->srcaddr;
    struct ep_source *this_src = this_addr->src;
    struct sockaddr_un addr;
    socklen_t len = sizeof(addr);
    this_src->state = 1;
    while (this_src->state) {
        printf("wait for connections...\n");

        // wait for events on the socket
        int r = poll(&this_src->ksrc, 1, -1);
        int fd = accept(this_src->ksrc.fd, (struct sockaddr *) &addr, &len);
        printf("accept %d\n", fd);

        // make a new table entry
        struct ep_address *a = ep_new_addr(ld->table);
        memcpy(a->addr, &addr, len);
        a->addrlen = len;

        // initialise source and dest
        struct ep_source *s = ep_new_src(ld->table, a->epid);
        s->ksrc.fd = fd;
        s->ksrc.events = POLLIN;
        struct ep_dest *d = ep_new_dest(ld->table, a->epid);
        d->fd = fd;

        // start new thread
        ep_create_reader(a, ld->notify_read, ld->notify_obj);

        // call the notify function
        ld->notify_accept(this_addr, ld->notify_obj);
    }
}


void ep_activate_acceptor(struct ep_table *t, int epid, notify_fn_t af, notify_fn_t rf, void *obj) {
    int err;
    printf("creating socket acceptor\n");

    // the acceptor requires an initialised addr and src
    struct ep_address *a = ep_table_addr(t, epid);
    struct ep_source *s = ep_table_src(t, epid);
    if (a && s) {

        // bind and listen on the socket
        err = bind(s->ksrc.fd, (struct sockaddr *) &a->addr, a->addrlen);
        if (err < 0) {
            perror("bind");
            return;
        }
        listen(s->ksrc.fd, 5);

        // data passed to thread
        struct ep_accept_data *ad = malloc(sizeof(struct ep_accept_data));
        ad->table = t;
        ad->srcaddr = a;
        ad->notify_obj = obj;
        ad->notify_accept = af;
        ad->notify_read = rf;

        // init source struct
        s->mem = ad;
        err = pthread_create(&s->thread, NULL, ep_thread_accept, s->mem);
        if (err) {
            perror("pthread_create");
        }
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
        int err = connect(a->src->ksrc.fd, (struct sockaddr *) &a->addr, a->addrlen);
        if (err == -1) {
            perror("connect");
            return;
        }

        // start new thread
        ep_create_reader(a, rf, obj);
    }
}
