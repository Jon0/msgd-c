#ifndef LIBMSG_CONNECTION_H
#define LIBMSG_CONNECTION_H

#include <libutil/map.h>
#include <libutil/stream.h>
#include <libshare/message.h>
#include <libsys/endpoint.h>
#include <libsys/thread.h>


struct msg_connection;


/*
 * function to handle new messages
 */
typedef int (*msg_message_recv_t)(struct msg_connection *, struct msgu_message *, void *);


/*
 * either local-filesystems, local-processes or remote connections
 */
struct msg_connection {
    msgs_mutex_t         read_mutex;
    msgs_mutex_t         write_mutex;
    msg_message_recv_t   recv_fn;
    void                *recv_arg;
    struct msgs_socket   socket;
    struct msgu_stream   stream;
    struct msgu_parser   parser;
    struct msgu_map      handles;
    struct msgu_message  read_part;
    struct msgu_message  write_part;
    volatile int         new_events;
    int                  next_handle;
};


/*
 * init by connecting or accepting
 */
void msg_connection_init(struct msg_connection *conn, struct msgs_socket *socket, msg_message_recv_t fn, void *arg);
int msg_connection_connect(struct msg_connection *conn, struct msgu_address *addr, msg_message_recv_t fn, void *arg);
void msg_connection_close(struct msg_connection *conn);


/*
 * short label for logging
 */
void msg_connection_label(char *buf, const struct msg_connection *conn);
void msg_connection_log(const struct msg_connection *conn, const struct msgu_message *msg, const char *extra);


/*
 * create a new handle to a shared resource
 */
int msg_connection_init_handle(struct msg_connection *conn, const struct msgu_string *share_name);
int msg_connection_read_handle(struct msg_connection *conn, int node_handle);


/*
 * reads available data from stream, calls callback if complete object is read
 */
int msg_connection_notify(struct msg_connection *conn);
int msg_connection_poll(struct msg_connection *conn);


/*
 * sends any update type
 */
int msg_connection_send_message(struct msg_connection *conn, int event_type, int data_type, union msgu_any_msg *data);


#endif
