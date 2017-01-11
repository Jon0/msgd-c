#ifndef TESTS_MAIN
#define TESTS_MAIN

typedef int (*test_fn)();


struct test_item {
    const char *name;
    test_fn run;
};

void report_test(const char *name, int result);



#endif
