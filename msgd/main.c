#include <stdio.h>

#include <libmsg/channel.h>


int main(int argc, const char *argv[]) {
    struct msg_server cs;
    msg_server_run(&cs, "msgd-local");
    return 0;
}
