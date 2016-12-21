#ifndef ENDPOINT_H
#define ENDPOINT_H

#include <netinet/in.h>

#include <libutil/address.h>
#include <libutil/buffer.h>


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
int msgs_open_acceptor(struct msgs_acceptor *acc, struct msgu_address *addr);
int msgs_open_socket(struct msgs_socket *s, struct msgu_address *a);


int ep_notify_create();
int ep_notify_read(int infd);
void ep_notify_init(struct msgs_file *f, int infd, const char *path);
void ep_address_print(struct msgu_address *a);

#endif
