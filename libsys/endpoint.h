#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <netinet/in.h>

#include <libutil/address.h>
#include <libutil/buffer.h>


/*
 * converting os addresses to msgu address
 */
struct msgs_address {
    char               data [32];
    socklen_t          len;
};


struct msgs_acceptor {
    struct msgs_address addr;
    int                 fd;
};


/*
 * moves input from file descriptors into handlers
 */
struct msgs_socket {
    struct msgu_buffer   read_buf;
    struct msgu_buffer   write_buf;
    struct msgs_address  addr;
    int                  fd;
};


/*
 * monitor changes to an existing file system
 */
struct msgs_file {
    int wd;
};


/*
 * cleanup old sockets
 */
void ep_unlink(const char *address);


/*
 * set endpoint to listen for local connections
 */
void ep_local(struct msgu_address *a, const char *address);
void ep_listen_remote(struct msgu_address *a, short portnum);
void ep_connect_remote(struct msgu_address *a, const char *ip, short portnum);

/*
 * adds source and dest to table
 */
void ep_init_acceptor(struct msgs_acceptor *a);
int ep_init_channel(struct msgs_socket *s);


int ep_notify_create();
int ep_notify_read(int infd);
void ep_notify_init(struct msgs_file *f, int infd, const char *path);
void ep_address_print(struct msgs_address *a);

#endif
