#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/inotify.h>

#include "endpoint.h"


ssize_t msgs_socket_read_fn(msgu_stream_id_t id, void *buf, size_t count) {
    if (count == 0) {
        printf("read: count = 0\n");
        return 0;
    }
    ssize_t e = read(id.fd, buf, count);
    if (e == -1) {
        int err = errno;
        if ((err == EAGAIN) || (err == EWOULDBLOCK)) {
            return msgu_stream_waiting;
        }
        else {
            perror("read");
            return msgu_stream_operation_error;
        }
    }
    else if (e == 0) {
        printf("read: remote closed socket\n");
        return msgu_stream_remote_closed;
    }
    else {
        return e;
    }
}


ssize_t msgs_socket_write_fn(msgu_stream_id_t id, const void *buf, size_t count) {
    if (count == 0) {
        printf("write: count = 0\n");
        return 0;
    }
    ssize_t e = write(id.fd, buf, count);
    if (e == -1) {
        int err = errno;
        if ((err == EAGAIN) || (err == EWOULDBLOCK)) {
            return msgu_stream_waiting;
        }
        else {
            perror("write");
            return msgu_stream_operation_error;
        }
    }
    else {
        return e;
    }
}


ssize_t msgs_recv(struct msgu_buffer *b, int fd) {
    char *back;
    size_t space = 0;
    ep_buffer_endmem(b, &back, &space);

    // try read from file descriptor
    ssize_t r = read(fd, back, space);
    if (r < 0) {
        perror("read");
    }
    else {
        b->size += r;
    }
    return r;
}


size_t msgs_recv_src(struct msgu_buffer *b, int fd, size_t count) {
    char *back;
    size_t space = 0;
    ep_buffer_endmem(b, &back, &space);

    if (count > space) {
        count = space;
    }

    ssize_t r = read(fd, back, count);
    if (r < 0) {
        perror("read");
        return 0;
    }
    else {
        b->size += r;
        return r;
    }
}


ssize_t msgs_send(struct msgu_buffer *b, int fd, size_t begin) {

    // write as much as possible from begin
    size_t end = b->begin + b->size;
    if (end < b->avail) {
        return write(fd, &b->ptr[begin], end - begin);
    }
    else if (begin < (end - b->avail)) {
        // begin is positioned before end, only one write is required
        return write(fd, &b->ptr[begin], (end - b->avail) - begin);
    }
    else {
        // begin is positioned after end, not all can be written
        return write(fd, &b->ptr[begin], (b->begin - b->avail));
    }
}


size_t msgs_send_inc(struct msgu_buffer *b, int fd, size_t *begin) {
    ssize_t w = msgs_send(b, fd, *begin);
    if (w > 0) {
        *begin = (*begin + w) % b->avail;
    }

    // return remainder
    return ((b->avail + b->begin + b->size) - *begin) % b->avail;
}



int msgs_set_non_blocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        flags = 0;
    }
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}


void ep_unlink(const char *address) {
    unlink(address);
}


void msgs_address_print(char *buf, const struct msgu_address *a) {
    struct sockaddr *sa = (struct sockaddr *) &a->data;
    if (sa->sa_family == AF_UNIX) {
        struct sockaddr_un *su = (struct sockaddr_un *) &a->data;
        strcpy(buf, su->sun_path);
    }
    else if (sa->sa_family == AF_INET) {
        struct sockaddr_in *si = (struct sockaddr_in *) &a->data;
        char *addr = (char *) &si->sin_addr;
        sprintf(buf, "%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]);
    }
    else {
        sprintf(buf, "unknown");
    }
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


void msgu_init_remote_addr(struct msgu_address *a, const char *ip, short portnum) {
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
    msgs_set_non_blocking(acc->fd);
    return acc->fd;
}


int msgs_accept_socket(struct msgs_acceptor *acc, struct msgs_socket *s) {
    struct sockaddr *sa = (struct sockaddr *) &s->addr.data;
    s->addr.len = sizeof(s->addr.data);
    s->fd = accept(acc->fd, sa, (socklen_t *) &s->addr.len);
    if (s->fd == -1) {
        if (errno != EAGAIN) {
            perror("accept");
        }
        return 0;
    }

    s->addr.len = sizeof(s->addr.data);
    getsockname(s->fd, sa, (socklen_t *) &s->addr.len);
    msgs_set_non_blocking(s->fd);
    return s->fd;
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
    msgs_set_non_blocking(s->fd);
    return s->fd;
}


int msgs_close_socket(struct msgs_socket *s) {
    return close(s->fd);
}


int ep_notify_read(int infd) {
    struct inotify_event event;
    ssize_t len = read(infd, (char *) &event, sizeof(event));
    if (len == -1) {
        if (errno != EAGAIN) {
            perror("read");
        }
        return 0;
    }
    return event.wd;
}


void ep_notify_init(struct msgs_file *f, int infd, const char *path) {
    inotify_add_watch(f->wd, path, IN_OPEN | IN_CLOSE);
}
