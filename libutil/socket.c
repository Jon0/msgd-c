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


void ep_set_local(struct ep_source *src, const char *address) {

    // create a unix socket
    src->ksrc.fd = socket(AF_UNIX, SOCK_STREAM, 0);
    src->ksrc.events = POLLIN;
    if (src->ksrc.fd < 0) {
        perror("socket");
        return;
    }

    // set address name
    struct sockaddr_un *saun = (struct sockaddr_un *) &src->addr;
    saun->sun_family = AF_UNIX;
    strcpy(saun->sun_path, address);
    src->addrlen = sizeof(saun->sun_family) + strlen(saun->sun_path);
}


void *ep_thread_accept(void *p) {
    struct ep_accept_data *ld = (struct ep_accept_data *) p;
    struct ep_source *this_source = ld->tbsrc;
    struct sockaddr_un addr;
    socklen_t len = sizeof(addr);
    this_source->state = 1;
    while (this_source->state) {
        printf("wait for events...\n");

        // wait for events on the socket
        int r = poll(&this_source->ksrc, 1, -1);
        int fd = accept(this_source->ksrc.fd, (struct sockaddr *) &addr, &len);

        // make a new table entry
        struct ep_source *s = ep_table_add(ld->table);
        memcpy(s->addr, &addr, len);
        s->addrlen = len;

        // start new thread
        ep_create_reader(s, fd, ld->notify_read);

        // call the notify function
        ld->notify_accept(this_source);
    }
}


void ep_activate_acceptor(struct ep_table *t, struct ep_source *s, notify_fn_t af, notify_fn_t rf) {
    int err;
    printf("creating socket acceptor\n");

    // bind and listen on the socket
    err = bind(s->ksrc.fd, (struct sockaddr *) &s->addr, s->addrlen);
    if (err < 0) {
        perror("bind");
        return;
    }
    listen(s->ksrc.fd, 5);

    // data passed to thread
    struct ep_accept_data *ad = malloc(sizeof(struct ep_accept_data));
    s->mem = ad;
    ad->table = t;
    ad->tbsrc = s;
    ad->notify_accept = af;
    ad->notify_read = rf;

    err = pthread_create(&s->thread, NULL, ep_thread_accept, s->mem);
    if (err) {
        perror("pthread_create");
    }
}


void ep_activate_connector(struct ep_source *s, notify_fn_t rf) {
    printf("creating socket connector\n");

    int err = connect(s->ksrc.fd, (struct sockaddr *) &s->addr, s->addrlen);
    if (err == -1) {
        perror("connect");
        return;
    }

    // start new thread
    ep_create_reader(s, s->ksrc.fd, rf);
}
