#include <stdio.h>

#include "string.h"
#include "update.h"


const char *msgu_msgdata_str(const enum msgu_data_type type) {
    switch (type) {
    case msgdata_empty:
        return "empty";
    case msgdata_host_addr:
        return "host_addr";
    case msgdata_init_local:
        return "init_local";
    case msgdata_init_remote:
        return "init_remote";
    case msgdata_share_file:
        return "share_file";
    case msgdata_share_path:
        return "share_path";
    case msgdata_node_list:
        return "node_list";
    case msgdata_node_handle:
        return "node_handle";
    case msgdata_node_read:
        return "node_read";
    case msgdata_node_write:
        return "node_write";
    default:
        return "unknown";
    }
}


size_t msgu_empty_size(const struct msgu_empty_msg *u) {
    return 0;
}


int msgu_empty_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_empty_msg *u) {
    return msgu_stream_complete;
}


int msgu_empty_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_empty_msg *u) {
    return msgu_stream_complete;
}


size_t msgu_host_addr_size(const struct msgu_host_addr_msg *u) {
    return msgu_string_size(&u->address_str);
}


int msgu_host_addr_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_host_addr_msg *u) {
    return msgu_string_read_frag(stream, f, &u->address_str);
}


int msgu_host_addr_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_host_addr_msg *u) {
    return msgu_string_write_frag(stream, f, &u->address_str);
}


size_t msgu_init_local_size(const struct msgu_init_local_msg *u) {
    return sizeof(u->version_maj) + sizeof(u->version_min) + msgu_string_size(&u->proc_name);
}


int msgu_init_local_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_local_msg *u) {
    static msgu_transfer_read_t msgu_init_local_read_fns[] = {
        msgu_i32_read_frag,
        msgu_i32_read_frag,
        msgu_string_read_frag,
    };
    void *layout[] = {
        &u->version_maj,
        &u->version_min,
        &u->proc_name,
    };
    return msgu_read_many(stream, f, msgu_init_local_read_fns, layout, 3);
}


int msgu_init_local_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_init_local_msg *u) {
    static msgu_transfer_write_t msgu_init_local_write_fns[] = {
        msgu_i32_write_frag,
        msgu_i32_write_frag,
        msgu_string_write_frag,
    };
    const void *layout[] = {
        &u->version_maj,
        &u->version_min,
        &u->proc_name,
    };
    return msgu_write_many(stream, f, msgu_init_local_write_fns, layout, 3);
}


size_t msgu_init_remote_size(const struct msgu_init_remote_msg *u) {
    return sizeof(u->version_maj) + sizeof(u->version_min) + msgu_string_size(&u->host_name);
}


int msgu_init_remote_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_init_remote_msg *u) {
    static msgu_transfer_read_t msgu_init_remote_read_fns[] = {
        msgu_i32_read_frag,
        msgu_i32_read_frag,
        msgu_string_read_frag,
    };
    void *layout[] = {
        &u->version_maj,
        &u->version_min,
        &u->host_name,
    };
    return msgu_read_many(stream, f, msgu_init_remote_read_fns, layout, 3);
}


int msgu_init_remote_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_init_remote_msg *u) {
    static msgu_transfer_write_t msgu_init_remote_write_fns[] = {
        msgu_i32_write_frag,
        msgu_i32_write_frag,
        msgu_string_write_frag,
    };
    const void *layout[] = {
        &u->version_maj,
        &u->version_min,
        &u->host_name,
    };
    return msgu_write_many(stream, f, msgu_init_remote_write_fns, layout, 3);
}


size_t msgu_share_file_size(const struct msgu_share_file_msg *u) {
    return msgu_string_size(&u->share_name);
}


int msgu_share_file_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_file_msg *u) {
    static msgu_transfer_read_t msgu_add_share_read_fns[] = {
        msgu_string_read_frag,
    };
    void *layout[] = {
        &u->share_name,
    };
    return msgu_read_many(stream, f, msgu_add_share_read_fns, layout, 1);
}


int msgu_share_file_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_share_file_msg *u) {
    static msgu_transfer_write_t msgu_add_share_write_fns[] = {
        msgu_string_write_frag,
    };
    const void *layout[] = {
        &u->share_name,
    };
    return msgu_write_many(stream, f, msgu_add_share_write_fns, layout, 1);
}


size_t msgu_share_path_size(const struct msgu_share_path_msg *u) {
    return 0;
}


int msgu_share_path_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_share_path_msg *u) {
    return msgu_stream_complete;
}


int msgu_share_path_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_share_path_msg *u) {
    return msgu_stream_complete;
}


size_t msgu_node_list_size(const struct msgu_node_list_msg *u) {
    return msgu_queue_frag_size(&u->nodes);
}


int msgu_node_list_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_list_msg *u) {
    msgu_queue_init(&u->nodes, &msgu_node_element, sizeof(struct msgu_node));
    return msgu_queue_frag_read(stream, f, &u->nodes);
}


int msgu_node_list_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_node_list_msg *u) {
    return msgu_queue_frag_write(stream, f, &u->nodes);
}


size_t msgu_node_handle_size(const struct msgu_node_handle_msg *u) {
    return sizeof(u->node_handle);
}


int msgu_node_handle_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_handle_msg *u) {
    return msgu_read_fixed(stream, f, &u->node_handle, sizeof(u->node_handle));
}


int msgu_node_handle_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_node_handle_msg *u) {
    return msgu_write_fixed(stream, f, &u->node_handle, sizeof(u->node_handle));
}


size_t msgu_node_read_size(const struct msgu_node_read_msg *u) {
    return sizeof(u->node_handle) + sizeof(u->count);
}


int msgu_node_read_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_read_msg *u) {
    static msgu_transfer_read_t msgu_node_read_read_fns[] = {
        msgu_i32_read_frag,
        msgu_i32_read_frag,
    };
    void *layout[] = {
        &u->node_handle,
        &u->count,
    };
    return msgu_read_many(stream, f, msgu_node_read_read_fns, layout, 2);
}


int msgu_node_read_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_node_read_msg *u) {
    static msgu_transfer_write_t msgu_node_read_write_fns[] = {
        msgu_i32_write_frag,
        msgu_i32_write_frag,
    };
    const void *layout[] = {
        &u->node_handle,
        &u->count,
    };
    return msgu_write_many(stream, f, msgu_node_read_write_fns, layout, 2);
}


size_t msgu_node_write_size(const struct msgu_node_write_msg *u) {
    return sizeof(u->node_handle) + msgu_string_size(&u->data);
}


int msgu_node_write_read(struct msgu_stream *stream, struct msgu_fragment *f, struct msgu_node_write_msg *u) {
    static msgu_transfer_read_t msgu_node_write_read_fns[] = {
        msgu_i32_read_frag,
        msgu_string_read_frag,
    };
    void *layout[] = {
        &u->node_handle,
        &u->data,
    };
    return msgu_read_many(stream, f, msgu_node_write_read_fns, layout, 2);
}


int msgu_node_write_write(struct msgu_stream *stream, struct msgu_fragment *f, const struct msgu_node_write_msg *u) {
    static msgu_transfer_write_t msgu_node_write_write_fns[] = {
        msgu_i32_write_frag,
        msgu_string_write_frag,
    };
    const void *layout[] = {
        &u->node_handle,
        &u->data,
    };
    return msgu_write_many(stream, f, msgu_node_write_write_fns, layout, 2);
}


void msgu_msgdata_print(char *out, const struct msgu_msgdata *md) {
    const char *typename = msgu_msgdata_str(md->data_type);
    char arg_buffer [100];
    switch (md->data_type) {
    case msgdata_empty:
        sprintf(arg_buffer, "");
        break;
    case msgdata_host_addr:
        sprintf(arg_buffer, "%s", md->data.host_addr.address_str.buf);
        break;
    case msgdata_init_local:
        sprintf(arg_buffer, "");
        break;
    case msgdata_init_remote:
        sprintf(arg_buffer, "");
        break;
    case msgdata_share_file:
        sprintf(arg_buffer, "%s", md->data.share_file.share_name.buf);
        break;
    case msgdata_share_path:
        sprintf(arg_buffer, "");
        break;
    case msgdata_node_list:
        msgu_node_list_print(arg_buffer, &md->data.node_list.nodes);
        break;
    case msgdata_node_handle:
        sprintf(arg_buffer, "%d", md->data.node_handle.node_handle);
        break;
    case msgdata_node_read:
        sprintf(arg_buffer, "%d, %d", md->data.node_read.node_handle, md->data.node_read.count);
        break;
    case msgdata_node_write:
        sprintf(arg_buffer, "%d, %s", md->data.node_write.node_handle, md->data.node_write.data.buf);
        break;
    default:
        sprintf(arg_buffer, "unknown");
        break;
    }
    sprintf(out, "%s (%s)", typename, arg_buffer);
}


void msgu_msgdata_free(struct msgu_msgdata *md) {
    switch (md->data_type) {
    case msgdata_empty:
        break;
    case msgdata_host_addr:
        break;
    case msgdata_init_local:
        break;
    case msgdata_init_remote:
        break;
    case msgdata_share_file:
        break;
    case msgdata_share_path:
        break;
    case msgdata_node_list:
        break;
    case msgdata_node_handle:
        break;
    case msgdata_node_read:
        break;
    case msgdata_node_write:
        break;
    }
}


size_t msgu_msgdata_size(int data_type, const union msgu_any_msg *data) {
    switch (data_type) {
    case msgdata_empty:
        return msgu_empty_size(&data->empty);
    case msgdata_init_local:
        return msgu_init_local_size(&data->init_local);
    case msgdata_host_addr:
        return msgu_host_addr_size(&data->host_addr);
    case msgdata_init_remote:
        return msgu_init_remote_size(&data->init_remote);
    case msgdata_share_file:
        return msgu_share_file_size(&data->share_file);
    case msgdata_share_path:
        return msgu_share_path_size(&data->share_path);
    case msgdata_node_list:
        return msgu_node_list_size(&data->node_list);
    case msgdata_node_handle:
        return msgu_node_handle_size(&data->node_handle);
    case msgdata_node_read:
        return msgu_node_read_size(&data->node_read);
    case msgdata_node_write:
        return msgu_node_write_size(&data->node_write);
    default:
        return 0;
    }
}


size_t msgu_msgdata_size_frag(const void *md) {
    const struct msgu_msgdata *msgdata = md;
    return msgu_msgdata_size(msgdata->data_type, &msgdata->data);
}


int msgu_msgdata_read_frag(struct msgu_stream *in, struct msgu_fragment *f, void *md) {
    struct msgu_msgdata *msgdata = md;
    int read;
    if (f->index == 0) {
        read = msgu_read_fixed(in, &f[1], &msgdata->data_type, sizeof(msgdata->data_type));
        if (read == msgu_stream_complete) {
            msgu_fragment_inc(f);
        }
    }
    if (f->index == 1) {
        switch (msgdata->data_type) {
        case msgdata_empty:
            read = msgu_empty_read(in, &f[1], &msgdata->data.empty);
            break;
        case msgdata_host_addr:
            read = msgu_host_addr_read(in, &f[1], &msgdata->data.host_addr);
            break;
        case msgdata_init_local:
            read = msgu_init_local_read(in, &f[1], &msgdata->data.init_local);
            break;
        case msgdata_init_remote:
            read = msgu_init_remote_read(in, &f[1], &msgdata->data.init_remote);
            break;
        case msgdata_share_file:
            read = msgu_share_file_read(in, &f[1], &msgdata->data.share_file);
            break;
        case msgdata_share_path:
            read = msgu_share_path_read(in, &f[1], &msgdata->data.share_path);
            break;
        case msgdata_node_list:
            read = msgu_node_list_read(in, &f[1], &msgdata->data.node_list);
            break;
        case msgdata_node_handle:
            read = msgu_node_handle_read(in, &f[1], &msgdata->data.node_handle);
            break;
        case msgdata_node_read:
            read = msgu_node_read_read(in, &f[1], &msgdata->data.node_read);
            break;
        case msgdata_node_write:
            read = msgu_node_write_read(in, &f[1], &msgdata->data.node_write);
            break;
        default:
            read = msgu_stream_format_error;
            break;
        }
    }
    return read;
}


int msgu_msgdata_write_frag(struct msgu_stream *out, struct msgu_fragment *f, const void *md) {
    const struct msgu_msgdata *msgdata = md;
    int written;
    if (f->index == 0) {
        written = msgu_write_fixed(out, &f[1], &msgdata->data_type, sizeof(msgdata->data_type));
        if (written == msgu_stream_complete) {
            msgu_fragment_inc(f);
        }
    }
    if (f->index == 1) {
        switch (msgdata->data_type) {
        case msgdata_empty:
            written = msgu_empty_write(out, &f[1], &msgdata->data.empty);
            break;
        case msgdata_host_addr:
            written = msgu_host_addr_write(out, &f[1], &msgdata->data.host_addr);
            break;
        case msgdata_init_local:
            written = msgu_init_local_write(out, &f[1], &msgdata->data.init_local);
            break;
        case msgdata_init_remote:
            written = msgu_init_remote_write(out, &f[1], &msgdata->data.init_remote);
            break;
        case msgdata_share_file:
            written = msgu_share_file_write(out, &f[1], &msgdata->data.share_file);
            break;
        case msgdata_share_path:
            written = msgu_share_path_write(out, &f[1], &msgdata->data.share_path);
            break;
        case msgdata_node_list:
            written = msgu_node_list_write(out, &f[1], &msgdata->data.node_list);
            break;
        case msgdata_node_handle:
            written = msgu_node_handle_write(out, &f[1], &msgdata->data.node_handle);
            break;
        case msgdata_node_read:
            written = msgu_node_read_write(out, &f[1], &msgdata->data.node_read);
            break;
        case msgdata_node_write:
            written = msgu_node_write_write(out, &f[1], &msgdata->data.node_write);
            break;
        default:
            written = msgu_stream_format_error;
            break;
        }
    }
    return written;
}
