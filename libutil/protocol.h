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
    enum msgu_msgtype type;
    int32_t           host_id;
    int32_t           network_id;
    int32_t           share_id;
    int32_t           size;
};


/*
 * state of message reading
 */
struct msgu_read_status {
    struct msgu_header header;
    size_t header_read;
    size_t message_read;
    struct msgu_fragment fragment;
};



/*
 * verify a buffer contains a valid message
 */
int msg_invalid_buffer(struct msgu_buffer *in);


/*
 * resets state to read next header
 */
void msgu_stat_init(struct msgu_read_status *stat);
void msgu_stat_reset(struct msgu_read_status *stat);


/*
 * take next message header from incoming stream
 */
int msgu_poll_header(struct msgu_stream *in, struct msgu_read_status *stat);
int msgu_push_header(struct msgu_stream *out, struct msgu_fragment *f, enum msgu_msgtype id, int32_t length);


/*
 * read update, return type of update or 0 for incomplete read, and -1 for errors
 */
int msgu_poll_update(struct msgu_stream *in, struct msgu_read_status *stat, union msgu_any_update *update);
int msgu_push_update(struct msgu_stream *out, struct msgu_fragment *f, int update_type, union msgu_any_update *update);


/*
 * request types
 * unused?
 */
void msg_req_share(struct msgu_buffer *b, const char *path);
void msg_req_peer_init(struct msgu_stream *s, struct msg_host *h);
void msg_req_proc_init(struct msgu_stream *s, const char *msg, size_t count);


/*
 * send and recieve objects
 */
void msg_send_host(struct msg_host *h, struct msgu_stream *s);
void msg_send_host_list(struct msg_host_list *h, struct msgu_stream *s);


#endif
