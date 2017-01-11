#include <stdio.h>
#include <string.h>

#include "main.h"
#include "test_channel.h"


static struct test_item tests[] = {
    { .name = "channel_test_1", .run = channel_test_1 },
};



void report_test(const char *name, int result) {
    if (result == 1) {
        printf("%s: pass\n", name);
    }
    else {
        printf("%s: fail\n", name);
    }
}


void print_usage() {
    printf("msgd-test\n");
}


int main(int argc, char *argv[]) {
    size_t count = sizeof(tests) / sizeof(struct test_item);
    for (int i = 0; i < count; ++i) {
        report_test(tests[i].name, tests[i].run());
    }
}
