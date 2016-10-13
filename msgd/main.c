#include <stdio.h>

#include <libmsg/channel.h>


int main(int argc, const char *argv[]) {
    struct msg_server cs;
    msg_server_init(&cs, "msgd-local");
    msg_server_run(&cs);
    return 0;
}
