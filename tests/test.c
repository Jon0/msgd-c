#include <string.h>

#include <libmsg/msgd.h>

int main() {
    struct msg_client_state nstate;
    struct node_id_set nds;
    msg_init_proc(&nstate, "test", 0);

    // get available nodes
    msg_available(&nstate, &nds);


    msg_free_proc(&nstate);
    return 0;
}
