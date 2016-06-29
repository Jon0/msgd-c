#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "socket.h"


void sk_open(char *address) {
    int sd;
    struct sockaddr_un saun, fsaun;
    if ((sd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
    }


    saun.sun_family = AF_UNIX;
    strcpy(saun.sun_path, address);
    unlink(address);
    int len = sizeof(saun.sun_family) + strlen(saun.sun_path);

    if (bind(sd, (struct sockaddr *) &saun, len) < 0) {
        perror("server: bind");
        exit(1);
    }
}


void *ep_thread_accept(void *p) {
    struct ep_accept_data *ld = (struct ep_accept_data *) p;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    while (1) {
        // wait for events on the socket
        int r = poll(&ld->src, 1, -1);

        // add entry to endpoint table

        // start new connection threads
        // requires mem allocation for new thread
        int fd = accept(ld->src.fd, (struct sockaddr *) &addr, &len);

        // call the notify function
        ld->notify(NULL);
    }
}


void ep_create_acceptor(struct ep_source *s, notify_fn_t fn) {
    printf("creating socket acceptor\n");

    // create a unix socket
    struct ep_accept_data *ad = malloc(sizeof(struct ep_accept_data));
    s->mem = ad;
    ad->src.fd = socket(AF_UNIX, SOCK_STREAM, 0);
    ad->src.events = POLLIN;
    ad->notify = fn;


    int err = pthread_create(&s->thread, NULL, ep_thread_accept, s->mem);
    if (err) {
        perror("pthread_create");
    }

}
