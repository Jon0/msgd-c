#include <stdio.h>

#include <netinet/in.h>
#include <unistd.h>

#include "node.h"
#include "update.h"

void check(const char *msg) {
    perror(msg);
}


void list_updates(struct node_base *src, struct node_update *result, int64_t size) {
    char buffer [1024];
    if (src->mode) {
        struct node_src *nsrc = (struct node_src *) src;


        // poll the file descriptor for available input
        result[0].in_fd = nsrc->fd;

        //

    }
    else {
        struct node_tree *tsrc = (struct node_tree *) src;
    }
}


int update_inputs(struct node_base *result, const char *name) {
    // poll each input, and collect outputs to write
}


int update_outputs(struct node_base *result, const char *name) {
    // update or create the collection of output file descriptors
    // by resolving each transformation node

}


void main_loop() {
    int64_t loop = 1;
    while (loop) {
        // for each registered process
        // take new data and apply the default transform

        // update the queue the process polls from
        //

        // listen for new hosts
    }
}


int main(int argc, const char *argv[]) {
    struct node_buffer buffer;
    init_node_buffer(&buffer, 1024);
    free_node_buffer(&buffer);
}
