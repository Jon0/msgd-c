#include <stdio.h>

#include <libutil/map.h>
#include <libutil/tree.h>


void map_test() {
    struct msgu_map map;
    msgu_map_init(&map, msgu_int_hash, msgu_int_cmp, sizeof(int), sizeof(int));
    msgu_map_alloc(&map, 32);

    // should return NULL
    int key = 123;
    int value = 35;
    void *x = msgu_map_get(&map, &key);
    printf("map get %lx\n", (unsigned long) x);


    msgu_map_insert(&map, &key, &value);
    x = msgu_map_get(&map, &key);
    printf("map get %lx\n", (unsigned long) x);

    // should return null again
    msgu_map_erase(&map, &key);
    x = msgu_map_get(&map, &key);
    printf("map get %lx\n", (unsigned long) x);

    // cannot remove twice
    int err = msgu_map_erase(&map, &key);
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


    struct msgu_buffer buf;
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
