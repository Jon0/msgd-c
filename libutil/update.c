#include <stdio.h>

#include "string.h"
#include "update.h"


size_t msgu_init_local_size(struct msgu_init_local_update *u) {
    return 0;
}


int msgu_init_local_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_update *u) {
    return 0;
}


int msgu_init_local_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_update *u) {
    return 0;
}


size_t msgu_init_remote_size(struct msgu_init_remote_update *u) {
    return 0;
}


int msgu_init_remote_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_update *u) {
    return 0;
}


int msgu_init_remote_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_update *u) {
    return 0;
}



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


size_t msgu_update_size(int type, union msgu_any_update *u) {
    switch (type) {
    case msg_type_init_local:
        return msgu_init_local_size(&u->init_local);
    case msg_type_init_remote:
        return msgu_init_remote_size(&u->init_remote);
    case msg_type_add_share:
        return msgu_add_share_size(&u->sh_add);
    default:
        return 0;
    }
}


void msgu_update_print(int type, union msgu_any_update *u) {
    switch (type) {
    case msg_type_init_local:
        printf("update: init local\n");
        break;
    case msg_type_init_remote:
        printf("update: init remote\n");
        break;
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
