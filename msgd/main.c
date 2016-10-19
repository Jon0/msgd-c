#include <stdio.h>

#include <libmsg/channel.h>


int main(int argc, const char *argv[]) {
    struct msg_server cs;
    msg_server_init(&cs, "msgd-local");
    for (int i = 1; i < argc; ++i) {
        printf("connecting to %s\n", argv[i]);
        int id = msg_server_connect(&cs, argv[i]);
        if (id < 0) {
            printf("error connecting %s\n", argv[i]);
        }
    }
    msg_server_run(&cs);
    return 0;
}
