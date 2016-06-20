#include <msgd.h>

int main() {
    struct node_state nstate;
    md_init_node(&nstate, "test", 0);
    md_free_node(&nstate);
    return 0;
}
