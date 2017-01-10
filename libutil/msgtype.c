#include "msgtype.h"

const char *msgu_msgtype_str(enum msgu_msgtype type) {
    switch (type) {
    case msgtype_init_local:
        return "init_local";
    case msgtype_init_remote:
        return "init_remote";
    case msgtype_mount:
        return "mount";
    case msgtype_unmount:
        return "unmount";
    case msgtype_list_shares :
        return "list_shares";
    case msgtype_add_share_proc:
        return "add_share_proc";
    case msgtype_add_share_file:
        return "add_share_file";
    case msgtype_rm_share:
        return "rm_share";
    case msgtype_file_stream_read:
        return "file_stream_read";
    case msgtype_file_stream_write:
        return "file_stream_write";
    case msgtype_file_block_read:
        return "file_block_read";
    case msgtype_file_block_write:
        return "file_block_write";
    case msgtype_path_list:
        return "path_list";
    case msgtype_path_stream_read:
        return "path_stream_read";
    case msgtype_path_stream_write:
        return "path_stream_write";
    case msgtype_path_block_read:
        return "path_block_read";
    case msgtype_path_block_write:
        return "path_block_write";
    case msgtype_return_status:
        return "return_status";
    case msgtype_return_share_list:
        return "return_share_list";
    case msgtype_return_node_list:
        return "return_node_list";
    default:
        return "unknown";
    }
}
