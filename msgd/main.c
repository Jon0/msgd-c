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


void main_loop(int modc, struct module_private *mod) {
    struct node_buffer nbuffer;
    init_node_buffer(&nbuffer);
    int64_t loop = 1;
    while (loop) {
        update_node_buffer(&nbuffer);
        for (int i = 0; i < modc; ++i) {
            mod[i].update(&mod[i].state, &nbuffer);
        }
    }
    free_node_buffer(&nbuffer);
}


int main(int argc, const char *argv[]) {

    // open the required modules
    int mod_count = argc - 1;
    struct module_private *mp = malloc(sizeof(struct module_private) * mod_count);
    for (int i = 0; i < mod_count; ++i) {
        open_msgd_module(&mp[i], argv[i + 1]);
        printf("loaded module %s\n", mp->state.name);
    }

    // run main loop until exit
    main_loop(mod_count, mp);

    // cleanup
    for (int i = 0; i < mod_count; ++i) {
        close_msgd_module(&mp[i]);
    }
    free(mp);
}
