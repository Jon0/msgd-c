#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/inotify.h>

#include "endpoint.h"


void ep_unlink(const char *address) {
    unlink(address);
}


void ep_local(struct msgu_address *a, const char *address) {

    // intialise address struct with address name
    struct sockaddr_un *saun = (struct sockaddr_un *) &a->data;
    saun->sun_family = AF_UNIX;
    strcpy(saun->sun_path, address);
    a->len = sizeof(saun->sun_family) + strlen(saun->sun_path);
}


void ep_listen_remote(struct msgu_address *a, short portnum) {
    struct sockaddr_in *addr = (struct sockaddr_in *) &a->data;
    bzero(addr, sizeof(struct sockaddr_in));
    addr->sin_family = AF_INET;
    addr->sin_addr.s_addr = INADDR_ANY;
    addr->sin_port = htons(portnum);
    a->len = sizeof(struct sockaddr_in);
}


void ep_connect_remote(struct msgu_address *a, const char *ip, short portnum) {
    struct sockaddr_in *addr = (struct sockaddr_in *) &a->data;
    bzero(addr, sizeof(struct sockaddr_in));

    struct in_addr buf;
	if (inet_pton(AF_INET, ip, &buf) <= 0) {
        perror("inet_pton");
    }

    addr->sin_family = AF_INET;
    addr->sin_addr = buf;
    addr->sin_port = htons(portnum);
    a->len = sizeof(struct sockaddr_in);
}


int msgs_open_acceptor(struct msgs_acceptor *acc, struct msgu_address *addr) {
    acc->addr = *addr;
    struct sockaddr *sa = (struct sockaddr *) &acc->addr.data;
    acc->fd = socket(sa->sa_family, SOCK_STREAM, 0);
    if (acc->fd < 0) {
        perror("socket");
        return -1;
    }

    // bind and listen on the socket
    int err = bind(acc->fd, sa, acc->addr.len);
    if (err < 0) {
        perror("bind");
        close(acc->fd);
        return -1;
    }
    listen(acc->fd, 5);
    return acc->fd;
}


int msgs_open_socket(struct msgs_socket *s, struct msgu_address *a) {
    s->addr = *a;
    struct sockaddr *sa = (struct sockaddr *) &s->addr.data;
    s->fd = socket(sa->sa_family, SOCK_STREAM, 0);
    if (s->fd < 0) {
        perror("socket");
        return -1;
    }

    // connect to address
    int err = connect(s->fd, sa, s->addr.len);
    if (err == -1) {
        perror("connect");
        close(s->fd);
        return -1;
    }
    return s->fd;
}


int ep_notify_read(int infd) {
    struct inotify_event event;
    size_t len = read(infd, (char *) &event, sizeof(event));
    if (len == -1) {
        perror("read");
        return 0;
    }
    return event.wd;
}


void ep_notify_init(struct msgs_file *f, int infd, const char *path) {
    inotify_add_watch(f->wd, path, IN_OPEN | IN_CLOSE);
}


void ep_address_print(struct msgu_address *a) {
    struct sockaddr_in *s = (struct sockaddr_in *) &a->data;
    char *addr = (char *) &s->sin_addr;
    printf("%u.%u.%u.%u\n", addr[0], addr[1], addr[2], addr[3]);
}
