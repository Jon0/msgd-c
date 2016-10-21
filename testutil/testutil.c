#include <stdio.h>

#include <libutil/hashmap.h>
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

    int key = 123;
    ep_map_insert(&map, &key);
    x = ep_map_get(&map, key);
    printf("map get %x\n", x);

    // should return null again
    ep_map_erase(&map, key);
    x = ep_map_get(&map, key);
    printf("map get %x\n", x);

    // cannot remove twice
    int err = ep_map_erase(&map, key);
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
    ep_tree_read(&tree, &buf, 0);
    ep_tree_print(&tree);
}


int main(int argc, char *argv[]) {
    map_test();
    tree_test();
    return 0;
}
