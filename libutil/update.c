#include <stdio.h>

#include "string.h"
#include "update.h"


size_t msgu_empty_size(struct msgu_empty_update *u) {
    return 0;
}


int msgu_empty_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_empty_update *u) {
    return msgu_stream_complete;
}


int msgu_empty_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_empty_update *u) {
    return msgu_stream_complete;
}


size_t msgu_init_local_size(struct msgu_init_local_update *u) {
    return 0;
}


int msgu_init_local_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_update *u) {
    return msgu_stream_complete;
}


int msgu_init_local_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_update *u) {
    return msgu_stream_complete;
}


size_t msgu_init_remote_size(struct msgu_init_remote_update *u) {
    return 0;
}


int msgu_init_remote_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_update *u) {
    return msgu_stream_complete;
}


int msgu_init_remote_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_update *u) {
    return msgu_stream_complete;
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
    return msgu_read_many(stream, f, msgu_add_share_read_fns, layout, 1);
}


int msgu_share_file_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_file_update *u) {
    static msgu_frag_write_t msgu_add_share_write_fns[] = {
        msgu_string_write_frag,
    };
    const void *layout[] = {
        &u->share_name,
    };
    return msgu_write_many(stream, f, msgu_add_share_write_fns, layout, 1);
}


size_t msgu_node_list_size(struct msgu_node_list_update *u) {
    return msgu_queue_frag_size(&u->nodes);
}


int msgu_node_list_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_list_update *u) {
    msgu_queue_init(&u->nodes, &msgu_node_element, sizeof(struct msgu_node));
    return msgu_queue_frag_read(stream, f, &u->nodes);
}


int msgu_node_list_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_list_update *u) {
    return msgu_queue_frag_write(stream, f, &u->nodes);
}


size_t msgu_node_handle_size(struct msgu_node_handle_update *u) {
    return sizeof(u->node_handle);
}


int msgu_node_handle_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_handle_update *u) {
    return msgu_read_fixed(stream, f, &u->node_handle, sizeof(u->node_handle));
}


int msgu_node_handle_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_handle_update *u) {
    return msgu_write_fixed(stream, f, &u->node_handle, sizeof(u->node_handle));
}


size_t msgu_node_stream_size(struct msgu_node_stream_update *u) {
    return sizeof(u->node_handle) + msgu_string_size(&u->data);
}


int msgu_node_stream_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_stream_update *u) {
    static msgu_frag_read_t msgu_node_stream_read_fns[] = {
        msgu_i32_read_frag,
        msgu_string_read_frag,
    };
    void *layout[] = {
        &u->node_handle,
        &u->data,
    };
    return msgu_read_many(stream, f, msgu_node_stream_read_fns, layout, 2);
}


int msgu_node_stream_write(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_stream_update *u) {
    static msgu_frag_write_t msgu_node_stream_write_fns[] = {
        msgu_i32_write_frag,
        msgu_string_write_frag,
    };
    const void *layout[] = {
        &u->node_handle,
        &u->data,
    };
    return msgu_write_many(stream, f, msgu_node_stream_write_fns, layout, 2);
}



size_t msgu_update_size(int type, union msgu_any_update *u) {
    switch (type) {
    case msgtype_init_local:
        return msgu_init_local_size(&u->init_local);
    case msgtype_init_remote:
        return msgu_init_remote_size(&u->init_remote);
    case msgtype_add_share_file:
        return msgu_share_file_size(&u->share_file);
    case msgtype_file_open:
        return msgu_share_file_size(&u->share_file);
    case msgtype_return_share_list:
        return msgu_node_list_size(&u->node_list);
    case msgtype_return_node_handle:
        return msgu_node_handle_size(&u->node_handle);
    case msgtype_file_stream_read:
        return msgu_node_stream_size(&u->stream);
    case msgtype_file_stream_write:
        return msgu_node_stream_size(&u->stream);
    default:
        return 0;
    }
}


void msgu_update_print(int type, union msgu_any_update *u) {
    printf("update: %s, size: %lu\n", msgu_msgtype_str(type), msgu_update_size(type, u));
    switch (type) {
    case msgtype_add_share_file:
        printf("args: %s\n", u->share_file.share_name.buf);
        break;
    case msgtype_rm_share:
        printf("args: %s\n", u->share_file.share_name.buf);
        break;
    case msgtype_return_share_list:
        printf("args: %d\n", u->node_list.nodes.size);
        msgu_node_list_print(&u->node_list.nodes);
    case msgtype_return_node_handle:
        printf("args: %d\n", u->node_handle.node_handle);
    case msgtype_file_stream_read:
        printf("args: handle = %d, data = %s\n", u->stream.node_handle, u->stream.data.buf);
    case msgtype_file_stream_write:
        printf("args: handle = %d, data = %s\n", u->stream.node_handle, u->stream.data.buf);
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


int msgu_any_update_read(struct msgu_stream *in, struct msgu_fragment *f, int data_type, union msgu_any_update *data) {
    int read;
    switch (data_type) {
    case msgtype_init_local:
        read = msgu_init_local_read(in, f, &data->init_local);
        break;
    case msgtype_init_remote:
        read = msgu_init_remote_read(in, f, &data->init_remote);
        break;
    case msgtype_list_shares:
        read = msgu_empty_read(in, f, &data->empty);
        break;
    case msgtype_add_share_file:
        read = msgu_share_file_read(in, f, &data->share_file);
        break;
    case msgtype_file_open:
        read = msgu_share_file_read(in, f, &data->share_file);
        break;
    case msgtype_file_stream_read:
        read = msgu_node_stream_read(in, f, &data->stream);
        break;
    case msgtype_file_stream_write:
        read = msgu_node_stream_read(in, f, &data->stream);
        break;
    case msgtype_file_block_read:
        read = msgu_empty_read(in, f, &data->empty);
        break;
    case msgtype_file_block_write:
        read = msgu_empty_read(in, f, &data->empty);
        break;
    case msgtype_return_share_list:
        read = msgu_node_list_read(in, f, &data->node_list);
        break;
    case msgtype_return_node_handle:
        read = msgu_node_handle_read(in, f, &data->node_handle);
        break;
    default:
        printf("unknown update %d\n", data_type);
        read = msgu_stream_format_error;
        break;
    }
    return read;
}


int msgu_any_update_write(struct msgu_stream *out, struct msgu_fragment *f, int data_type, union msgu_any_update *data) {
    int written;
    switch (data_type) {
    case msgtype_init_local:
        written = msgu_init_local_write(out, f, &data->init_local);
        break;
    case msgtype_init_remote:
        written = msgu_init_remote_write(out, f, &data->init_remote);
        break;
    case msgtype_list_shares:
        written = msgu_empty_write(out, f, &data->empty);
        break;
    case msgtype_add_share_file:
        written = msgu_share_file_write(out, f, &data->share_file);
        break;
    case msgtype_file_open:
        written = msgu_share_file_write(out, f, &data->share_file);
        break;
    case msgtype_file_stream_read:
        written = msgu_node_stream_write(out, f, &data->stream);
        break;
    case msgtype_file_stream_write:
        written = msgu_node_stream_write(out, f, &data->stream);
        break;
    case msgtype_file_block_read:
        written = msgu_empty_write(out, f, &data->empty);
        break;
    case msgtype_file_block_write:
        written = msgu_empty_write(out, f, &data->empty);
        break;
    case msgtype_return_share_list:
        written = msgu_node_list_write(out, f, &data->node_list);
        break;
    case msgtype_return_node_handle:
        written = msgu_node_handle_write(out, f, &data->node_handle);
        break;
    default:
        printf("unknown update %d\n", data_type);
        written = msgu_stream_format_error;
        break;
    }
    return written;
}
