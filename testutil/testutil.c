#include <stdio.h>

#include <libutil/endpoint.h>
#include <libutil/hashmap.h>
#include <libutil/socket.h>
#include <libutil/thread.h>
#include <libutil/tree.h>


int test_key(void *p) {
    int *i = (int *) p;
    return *i;
}


void map_test() {
    struct ep_map map;
    ep_map_alloc(&map, test_key, sizeof(int), 32);

    // should return NULL
    void *x = ep_map_get(&map, 123);
    printf("map get %x\n", x);

    int number = 123;
    ep_map_insert(&map, &number);
    x = ep_map_get(&map, 123);
    printf("map get %x\n", x);

    // should return null again
    ep_map_erase(&map, 123);
    x = ep_map_get(&map, 123);
    printf("map get %x\n", x);

    // cannot remove twice
    int err = ep_map_erase(&map, 123);
    printf("map erase %d\n", err);
}


void tree_test() {
    struct ep_tree tree;
    ep_tree_alloc(&tree, sizeof(int), 256);
    ep_tree_link(&tree, -1, 0);
    ep_tree_print(&tree);

    printf("add subnodes\n\n");
    ep_tree_insert(&tree, 0);
    ep_tree_insert(&tree, 0);
    ep_tree_insert(&tree, 0);
    int l = ep_tree_insert(&tree, 1);
    ep_tree_insert(&tree, l);
    ep_tree_print(&tree);

    printf("remove subnodes\n\n");
    ep_tree_remove(&tree, l);
    ep_tree_print(&tree);


    struct ep_buffer buf;
    ep_buffer_init(&buf, malloc(1024), 1024);

    printf("write\n\n");
    ep_tree_write(&tree, &buf);

    printf("read\n\n");
    ep_tree_read(&tree, &buf);
    ep_tree_print(&tree);
}


void callback_test(int e, struct ep_event_view *v) {
    printf("event\n");
}


int accept_test(struct ep_table *t, int epid) {
    printf("accept test\n");
    return 0;
}


void thread_test() {
    struct ep_thread_pool tp;
    struct ep_table table;
    ep_table_init(&table, 16);
    ep_thread_pool_create(&tp, &table, 4, 0);


    // listen for incoming events
    ep_queue_from_table(&tp.queue);
    ep_thread_pool_join(&tp);
    ep_table_free(&table);
}


int main(int argc, char *argv[]) {
    map_test();
    tree_test();
    return 0;
}
