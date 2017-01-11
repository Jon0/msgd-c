#include <stdio.h>
#include <string.h>


static test_item tests[] {
    { .name "1", .run = NULL },
};



void report_test(const char *name, int result) {

}




void print_usage() {
    printf("msgd-test\n");
}


int main(int argc, char *argv[]) {
    for (int i = 0; i < 0; ++i) {
        report_test(tests[i].name, tests[i].run());
    }
}
