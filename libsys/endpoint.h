#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <netinet/in.h>

#include <libutil/buffer.h>
#include <libutil/stream.h>
#include <libshare/address.h>


ssize_t msgs_socket_read_fn(msgu_stream_id_t id, void *buf, size_t count);
ssize_t msgs_socket_write_fn(msgu_stream_id_t id, const void *buf, size_t count);


static struct msgu_stream_fn msgs_socket_fn = {
    .read  = msgs_socket_read_fn,
    .write = msgs_socket_write_fn,
};


struct msgs_acceptor {
    int                 fd;
    struct msgu_address addr;
};


/*
 * read and write from sockets
 */
struct msgs_socket {
    int                  fd;
    struct msgu_address  addr;
};


/*
 * monitor changes to an existing file system
 */
struct msgs_file {
    int fd;
    int wd;
};


/*
 * recv available data from a readable file descriptor
 */
ssize_t msgs_recv(struct msgu_buffer *b, int fd);
size_t msgs_recv_src(struct msgu_buffer *b, int fd, size_t count);


/*
 * send data to a file descriptor, make no changes to the buffer
 */
ssize_t msgs_send(struct msgu_buffer *b, int fd, size_t begin);
size_t msgs_send_inc(struct msgu_buffer *b, int fd, size_t *begin);


/*
 * adds a non blocking flag to file descriptor
 */
int msgs_set_non_blocking(int fd);


/*
 * cleanup old sockets
 */
void ep_unlink(const char *address);


/*
 * ignore sigpipe
 */
void msgs_set_signals();


/*
 * print readable ip address
 */
void msgs_address_print(char *buf, const struct msgu_address *a);


/*
 * set endpoint to listen for local connections
 */
void ep_local(struct msgu_address *a, const char *address);
void ep_listen_remote(struct msgu_address *a, short portnum);
void msgu_init_remote_addr(struct msgu_address *a, const char *ip, short portnum);

/*
 * adds source and dest to table
 */
int msgs_open_acceptor(struct msgs_acceptor *acc, struct msgu_address *addr);
int msgs_accept_socket(struct msgs_acceptor *acc, struct msgs_socket *s);
int msgs_open_socket(struct msgs_socket *s, struct msgu_address *a);
int msgs_close_socket(struct msgs_socket *s);
int msgs_get_address(struct msgs_socket *s, struct msgu_address *a);


int ep_notify_create();
int ep_notify_read(int infd);
void ep_notify_init(struct msgs_file *f, int infd, const char *path);

#endif
