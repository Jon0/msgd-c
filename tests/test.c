#include <string.h>

#include <libmsg/msgd.h>

int main() {
    struct client_state nstate;
    struct node_id_set nds;
    md_init_proc(&nstate, "test", 0);

    // get available nodes
    md_available(&nstate, &nds);


    md_free_proc(&nstate);
    return 0;
}
