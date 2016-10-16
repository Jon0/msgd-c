#include <stdio.h>

#include <libmsg/channel.h>


int main(int argc, const char *argv[]) {
    struct msg_server cs;
    msg_server_init(&cs, "msgd-local");
    for (int i = 1; i < argc; ++i) {
        msg_server_connect(&cs, argv[i]);
    }
    msg_server_run(&cs);
    return 0;
}
