#include <stdio.h>
#include <string.h>

#include "protocol.h"


void msg_poll_apply(struct msg_server *srv, struct msg_request *r) {
    struct msg_message m;
    printf("recv update from ");
    ep_sink_print(r->src);


    // recieving requests to the local server
    size_t hs = sizeof(struct msg_header);
    size_t read_header;
    size_t read_body;
    while (r->buf->size >= hs) {
        read_header = ep_buffer_peek(r->buf, (char *) &m.head, 0, hs);
        if (read_header == hs) {
            read_body = ep_buffer_peek(r->buf, m.body, hs, m.head.size);
            if (read_body == m.head.size) {
                ep_buffer_release(r->buf, hs + m.head.size);
                msg_parse(srv, &m, r->src);
            }
        }
    }
}


void msg_parse(struct msg_server *srv, struct msg_message *m, struct ep_sink *out) {
    int newid;
    int *subints;
    printf("recv type %d (%d)\n", m->head.id, m->head.size);
    switch (m->head.id) {
    case msg_type_proc:
        newid = msg_tree_add_proc(&srv->tree, m->body, m->head.size);
        msg_server_connect(srv, out->epid, newid);
        msg_tree_send(&srv->tree, out);
        break;
    case msg_type_publ:
        newid = msg_node_of_host(srv, out->epid);
        msg_tree_subnode(&srv->tree, m->body, m->head.size, newid);
        msg_tree_send(&srv->tree, out);
        break;
    case msg_type_subs:
        subints = (int *) m->body;
        msg_server_subscribe(srv, subints[0], out->epid, subints[1]);
        msg_tree_send(&srv->tree, out);
    case msg_type_avail:
        msg_tree_send(&srv->tree, out);
        break;
    }
}


int msg_read(struct ep_table *t, int epid, struct msg_message *out) {
    // client reads replies
    int r = ep_table_read(t, epid, (char *) &out->head, sizeof(struct msg_header));
    if (r == sizeof(struct msg_header)) {
        printf("recv reply (%d, %d)\n", out->head.id, out->head.size);
        r = ep_table_read(t, epid, out->body, out->head.size);
        return 1;
    }
    return 0;
}


void msg_req_addproc(struct ep_buffer *b, const char *msg, size_t count) {
    struct msg_header head;
    head.id = msg_type_proc;
    head.size = count;
    printf("request add %s\n", msg);
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, msg, count);
}


void msg_req_avail(struct ep_buffer *b, struct ep_tree *t) {
    struct msg_header head;
    head.id = msg_type_avail;
    head.state = msg_tree_hash(t);
    head.size = 0;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
}


void msg_req_publish(struct ep_buffer *b, const char *name, size_t len, int nodeid) {
    struct msg_header head;
    head.id = msg_type_publ;
    head.size = len;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, name, len);
}


void msg_req_subscribe(struct ep_buffer *b, int nodeid, int subid) {
    struct msg_header head;
    head.id = msg_type_subs;
    head.size = sizeof(int) * 2;
    ep_buffer_insert(b, (char *) &head, sizeof(struct msg_header));
    ep_buffer_insert(b, (char *) &nodeid, sizeof(int));
    ep_buffer_insert(b, (char *) &subid, sizeof(int));
}


void msg_tree_send(struct ep_tree *tree, struct ep_sink *out) {
    ep_tree_send(tree, out);
}
