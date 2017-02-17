#ifndef LIBMSG_CONNECTION_H
#define LIBMSG_CONNECTION_H

#include <libutil/map.h>
#include <libutil/stream.h>
#include <libshare/message.h>
#include <libsys/endpoint.h>
#include <libsys/cache.h>
#include <libsys/thread.h>


struct msg_connection;


/*
 * function to handle new messages
 * updates the state of server
 */
typedef int (*msg_message_recv_t)(struct msg_connection *, struct msgu_message *, void *);


/*
 * either local-filesystems, local-processes or remote connections
 * uses buffer for loopback, and sockets for external
 */
struct msg_connection {
    int                  connection_id;
    msgs_mutex_t         read_mutex;
    msgs_mutex_t         write_mutex;
    msg_message_recv_t   recv_fn;
    void                *recv_arg;
    struct msgs_socket   socket;
    struct msgu_stream   stream;
    struct msgu_parser   parser;
    struct msgu_message  read_part;
    struct msgu_message  write_part;
    struct msgu_string   remote_name;
    volatile int         new_events;
    int                  next_handle;
};


/*
 * init by connecting or accepting
 */
void msg_connection_init(struct msg_connection *conn, int id, struct msgs_socket *socket, msg_message_recv_t fn, void *arg);
int msg_connection_connect(struct msg_connection *conn, int id, struct msgu_address *addr, msg_message_recv_t fn, void *arg);
void msg_connection_set_name(struct msg_connection *conn, const struct msgu_string *name);
void msg_connection_close(struct msg_connection *conn);


/*
 * short label for logging
 */
void msg_connection_label(char *buf, const struct msg_connection *conn);
void msg_connection_log(const struct msg_connection *conn, const struct msgu_message *msg, const char *extra);


/*
 * create a new handle to a shared resource
 */
int msg_connection_init_handle(struct msg_connection *conn, struct msgs_file_cache *c, const struct msgu_string *name);
int msg_connection_read_handle(struct msg_connection *conn, struct msgs_file_cache *c, int hdl, char *buf, size_t count);


/*
 * reads available data from stream, calls callback if complete object is read
 */
int msg_connection_notify(struct msg_connection *conn);
int msg_connection_poll(struct msg_connection *conn);


/*
 * sends any update type
 */
int msg_connection_send_message(struct msg_connection *conn, int event_id, int event_type, int data_type, const union msgu_any_msg *data);


#endif
