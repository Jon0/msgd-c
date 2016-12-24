#include "network.h"

void msgu_any_request(struct msgu_link *l) {

}


void msg_server_reply(struct msg_server *serv, int src_epid, struct msg_connection *conn) {
    struct msg_message msg;
    while(msg_poll_message(&conn->read_buf, &msg)) {
        msg_server_apply(serv, src_epid, &msg, &conn->write_buf);
        ep_buffer_release(msg.body, msg.head.size);
    }
}
