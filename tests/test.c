#include <msgd.h>

int main() {
    struct node_state nstate;
    md_init_proc(&nstate, "test", 0);
    md_free_proc(&nstate);
    return 0;
}
