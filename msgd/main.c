#include <stdio.h>

#include <libmsg/channel.h>


void open_module(struct msg_channel_state *cs, const char *path) {
    printf("loaded module %s\n", path);

    // open the required modules
    //struct module_private *mp = malloc(sizeof(struct module_private) * modc);
}


int main(int argc, const char *argv[]) {
    struct msg_channel_state cs;

    int modc = argc - 1;
    for (int i = 0; i < modc; ++i) {
        open_module(&cs, argv[i + 1]);
    }

}
