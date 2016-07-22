#include <stdio.h>

#include <libutil/endpoint.h>
#include <libutil/socket.h>
#include <libutil/thread.h>


void callback_test(int e, struct ep_event_view *v) {
    printf("event\n");
}


void thread_test() {
    struct ep_thread_pool tp;
    struct ep_table table;
    ep_table_init(&table);
    ep_thread_pool_create(&tp, &table, 4);


    // create a socket acceptor
    struct ep_handler *h = ep_new_hdl(&table, callback_test);
    ep_tcp_acceptor(&table, h, callback_test);


    // listen for incoming events
    ep_queue_from_table(&tp.queue, &table);
    ep_thread_pool_join(&tp);
    ep_table_free(&table);
}


int main(int argc, char *argv[]) {
    thread_test();
    return 0;
}
