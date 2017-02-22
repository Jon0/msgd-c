#include <stdio.h>

#include <libmsg/process.h>


int main(int argc, const char *argv[]) {
    struct msg_process cs;
    msg_main_init(&cs, "msgd-local");
    for (int i = 1; i < argc; ++i) {
        printf("connecting to %s\n", argv[i]);
        int id = msg_server_connect(&cs.state, argv[i]);
        if (id < 0) {
            printf("error connecting %s\n", argv[i]);
        }
    }
    msg_main_run(&cs);
    printf("exiting\n");
    return 0;
}
