#include <stdio.h>
#include <string.h>

#include "message.h"


void msgu_message_free(struct msgu_message *m) {

}


void msgu_message_print(const struct msgu_message *m) {

}


size_t msgu_message_size_frag(const void *m) {

}


int msgu_message_read_frag(struct msgu_stream *src, struct msgu_fragment *f, void *m) {

}


int msgu_message_write_frag(struct msgu_stream *dest, struct msgu_fragment *f, const void *m) {

}


hash_t msgu_message_hash(const void *m) {

}


int msgu_message_cmp(const void *a, const void *b) {

}
