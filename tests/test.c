#include <string.h>

#include <libmsg/msgd.h>

int main(int argc, char *argv[]) {
    struct msg_client_state nstate;
    struct msg_node_set nds;
    msg_connect(&nstate, "127.0.0.1", 2204);
    msg_init_proc(&nstate, "test", 0);

    // either connect or create a nodes
    // use arg as node name
    if (argc == 2) {

    }

    // get available nodes
    // block until result is recieved
    msg_available(&nstate, &nds);


    msg_free_proc(&nstate);
    return 0;
}
