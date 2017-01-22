#include <stdio.h>

#include "connection.h"


void msg_connection_init(struct msg_connection *conn, struct msgs_socket *socket, msg_message_recv_t fn, void *arg) {
    conn->recv_fn = fn;
    conn->recv_arg = arg;
    conn->socket = *socket;
    conn->new_events = 0;
    conn->next_handle = 0;
    msgs_mutex_init(&conn->read_mutex);
    msgs_mutex_init(&conn->write_mutex);
    msgu_stream_init(&conn->stream, &msgs_socket_fn, (msgu_stream_id_t) socket->fd);
    msgu_parser_init(&conn->parser, &msgu_message_transfer_fn);
}


int msg_connection_connect(struct msg_connection *conn, struct msgu_address *addr, msg_message_recv_t fn, void *arg) {
    if (msgs_open_socket(&conn->socket, addr)) {
        msg_connection_init(conn, &conn->socket, fn, arg);
        return 1;
    }
    else {
        return 0;
    }
}


void msg_connection_set_name(struct msg_connection *conn, const struct msgu_string *name) {
    conn->remote_name = *name;
}


void msg_connection_close(struct msg_connection *conn) {
    msgs_close_socket(&conn->socket);
    msgu_stream_free(&conn->stream);
}


void msg_connection_label(char *buf, const struct msg_connection *conn) {
    msgs_address_print(buf, &conn->socket.addr);
}


void msg_connection_log(const struct msg_connection *conn, const struct msgu_message *msg, const char *extra) {
    char label_print [32];
    char event_print [100];
    msg_connection_label(label_print, conn);
    msgu_message_print(event_print, msg);
    printf("[%s, %s] %s\n", label_print, extra, event_print);
}


int msg_connection_init_handle(struct msg_connection *conn, struct msgs_file_cache *c, const struct msgu_string *name) {
    struct msgs_cache_key cache_key;
    cache_key.remote_name = conn->remote_name;
    cache_key.handle_id = conn->next_handle++;
    msgs_file_handle(c, &cache_key, name);
    return cache_key.handle_id;
}


int msg_connection_read_handle(struct msg_connection *conn, struct msgs_file_cache *c, int hdl, char *buf, size_t count) {
    struct msgs_cache_key cache_key;
    cache_key.remote_name = conn->remote_name;
    cache_key.handle_id = hdl;
    return msgs_file_read(c, &cache_key, buf, count);
}


int msg_connection_notify(struct msg_connection *conn) {
    // TODO if not locked, the message may go unread
    // depending on the state of currently reading thread
    conn->new_events = 1;
}


int msg_connection_poll(struct msg_connection *conn) {
    // mutex has already been locked
    // read available data from socket
    while (1) {
        int result = msgu_parser_read(&conn->parser, &conn->stream, &conn->read_part);
        if (result == 1) {
            conn->recv_fn(conn, &conn->read_part, conn->recv_arg);
        }
        else {
            return result;
        }
    }
}


int msg_connection_send_message(struct msg_connection *conn, int event_type, int data_type, const union msgu_any_msg *data) {
    struct msgu_message *to_send = &conn->write_part;
    if (msgu_stream_is_open(&conn->stream)) {
        to_send->event_type = event_type;
        to_send->event_id = 123;
        to_send->data_size = msgu_msgdata_size(data_type, data);
        to_send->data_hash = 0x00ff00ff;
        to_send->buf.data_type = data_type;
        to_send->buf.data = *data;
        msg_connection_log(conn, to_send, "send");
        return msgu_parser_write(&conn->parser, &conn->stream, to_send);
    }
    else {
        printf("no connection\n");
        return -1;
    }
}
