#include <stdio.h>

#include <netinet/in.h>
#include <unistd.h>

#include <libutil/module.h>
#include <libmsg/node.h>
#include <libmsg/update.h>


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


void main_loop(int modc, const char *modpath[]) {
    struct node_buffer nbuffer;
    init_node_buffer(&nbuffer);

    // open the required modules
    struct module_private *mp = malloc(sizeof(struct module_private) * modc);
    for (int i = 0; i < modc; ++i) {
        open_msgd_module(&mp[i], &nbuffer, modpath[i]);
        printf("loaded module %s\n", mp->state.name);
    }

    // enter update loop
    int64_t loop = 1;
    while (loop) {
        update_node_buffer(&nbuffer);
    }


    // cleanup
    for (int i = 0; i < modc; ++i) {
        close_msgd_module(&mp[i]);
    }
    free(mp);
    free_node_buffer(&nbuffer);
}


int main(int argc, const char *argv[]) {

    // run main loop until exit
    main_loop(argc - 1, &argv[1]);
}
