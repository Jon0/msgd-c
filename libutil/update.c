#include <stdio.h>

#include "string.h"
#include "update.h"


/*
 * read and write lists
 */
static msgu_frag_read_t msgu_add_share_read_fns[] = {
    msgu_string_read_frag,
};


static msgu_frag_write_t msgu_add_share_write_fns[] = {
    msgu_string_write_frag,
};


size_t msgu_add_share_size(struct msgu_add_share_update *u) {
    return msgu_string_size(&u->share_name);
}


int msgu_add_share_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_add_share_update *u) {
    void *layout[] = {
        &u->share_name,
    };
    msgu_read_many(stream, f, msgu_add_share_read_fns, layout, 1);
}


int msgu_add_share_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_add_share_update *u) {
    const void *layout[] = {
        &u->share_name,
    };
    msgu_write_many(stream, f, msgu_add_share_write_fns, layout, 1);
}


int msgu_add_share_layout(struct msgu_add_share_update *u, void **l, size_t count) {
    if (count < 1) {
        return 0;
    }
    l[0] = &u->share_name;
}


void msgu_update_print(int type, union msgu_any_update *u) {
    switch (type) {
    case msg_type_add_share:
        printf("update: add share (%s)\n", u->sh_add.share_name.buf);
        break;
    default:
        printf("update: unknown\n");
        break;
    }
}


void msgu_update_free(int type, union msgu_any_update *u) {
    switch (type) {
    case msg_type_add_share:
        break;
    }
}
