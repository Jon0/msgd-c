#include "peer.h"


void init_peer_buffer(struct peer_buffer *pb) {
    init_mem_block(&pb->peer_buf, 1024);
}


void free_peer_buffer(struct peer_buffer *pb) {
    free_mem_block(&pb->peer_buf);
}


void update_peer_buffer(struct peer_buffer *pb, struct node_buffer *nd) {

}
