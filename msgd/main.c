#include <stdio.h>

#include <libmsg/channel.h>


int main(int argc, const char *argv[]) {
    struct msg_server_state cs;
    msg_server_run(&cs);
    return 0;
}
