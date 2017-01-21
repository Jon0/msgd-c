#include <stdio.h>

#include <libutil/map.h>
#include <libutil/tree.h>


void map_test_1() {
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
