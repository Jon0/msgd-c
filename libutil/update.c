#include <stdio.h>

#include "string.h"
#include "update.h"


size_t msgu_empty_size(struct msgu_empty_update *u) {
    return 0;
}


int msgu_empty_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_empty_update *u) {
    return 0;
}


int msgu_empty_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_empty_update *u) {
    return 0;
}


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


size_t msgu_share_file_size(struct msgu_share_file_update *u) {
    return msgu_string_size(&u->share_name);
}


int msgu_share_file_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_file_update *u) {
    static msgu_frag_read_t msgu_add_share_read_fns[] = {
        msgu_string_read_frag,
    };
    void *layout[] = {
        &u->share_name,
    };
    msgu_read_many(stream, f, msgu_add_share_read_fns, layout, 1);
}


int msgu_share_file_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_file_update *u) {
    static msgu_frag_write_t msgu_add_share_write_fns[] = {
        msgu_string_write_frag,
    };
    const void *layout[] = {
        &u->share_name,
    };
    msgu_write_many(stream, f, msgu_add_share_write_fns, layout, 1);
}


size_t msgu_update_size(int type, union msgu_any_update *u) {
    switch (type) {
    case msgtype_init_local:
        return msgu_init_local_size(&u->init_local);
    case msgtype_init_remote:
        return msgu_init_remote_size(&u->init_remote);
    case msgtype_add_share_file:
        return msgu_share_file_size(&u->share_file);
    default:
        return 0;
    }
}


void msgu_update_print(int type, union msgu_any_update *u) {
    printf("update: %s\n", msgu_msgtype_str(type));
    switch (type) {
    case msgtype_add_share_file:
        printf("args: %s\n", u->share_file.share_name.buf);
        break;
    case msgtype_rm_share:
        printf("args: %s\n", u->share_file.share_name.buf);
        break;
    }
}


void msgu_update_free(int type, union msgu_any_update *u) {
    switch (type) {
    case msgtype_add_share_file:
        break;
    default:
        break;
    }
}
