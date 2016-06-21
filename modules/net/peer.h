#ifndef HANDLER_H
#define HANDLER_H

#include <libmsg/mem.h>
#include <libmsg/node.h>


struct peer {
    int address;
};


struct peer_buffer {
    struct mem_block   peer_buf;
};


void init_peer_buffer(struct peer_buffer *pb);
void free_peer_buffer(struct peer_buffer *pb);
void update_peer_buffer(struct peer_buffer *pb, struct node_buffer *nd);


#endif
