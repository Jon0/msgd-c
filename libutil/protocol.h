#ifndef LIBUTIL_PROTOCOL_H
#define LIBUTIL_PROTOCOL_H

#include "buffer.h"
#include "host.h"
#include "update.h"


/*
 * TODO send a fixed width type
 */
typedef int32_t msg_int_t;


/*
 * messages to the server have this header
 */
struct msgu_header {
    enum msg_type_id type;
    int32_t host_id;
    int32_t network_id;
    int32_t share_id;
    int32_t size;
};


/*
 * state of message reading
 */
struct msgu_read_status {
    struct msgu_header header;
    size_t header_read;
    size_t message_read;
};



/*
 * verify a buffer contains a valid message
 */
int msg_invalid_buffer(struct msgu_buffer *in);


void msgu_stat_reset(struct msgu_read_status *stat);


/*
 * take next message header from incoming buffer
 */
int msgu_poll_header(struct msgu_stream *in, struct msgu_read_status *stat);


/*
 * read update
 */
int msgu_poll_update(struct msgu_stream *in, struct msgu_read_status *stat, union msgu_any_update *update);
int msgu_poll_update_share(struct msgu_stream *in, struct msgu_read_status *stat, union msgu_any_update *update);



/*
 * partial read
 * may depend on existing read
 */
 ssize_t msgu_add_peer(struct msgu_stream *in, size_t offset, struct msgu_peer_update *u);
 ssize_t msgu_add_share(struct msgu_stream *in, size_t offset, struct msgu_share_add_update *u);




/*
 * request types
 */
void msg_write_header(struct msgu_buffer *b, enum msg_type_id id, int32_t length);
void msg_req_share(struct msgu_buffer *b, const char *path);
void msg_req_peer_init(struct msgu_stream *s, struct msg_host *h);
void msg_req_proc_init(struct msgu_stream *s, const char *msg, size_t count);


/*
 * send and recieve objects
 */
size_t msg_send_block(struct msgu_buffer *buf, int share_id, int hdl, char *in, size_t count);
void msg_send_host(struct msg_host *h, struct msgu_stream *s);
void msg_send_host_list(struct msg_host_list *h, struct msgu_stream *s);


#endif
