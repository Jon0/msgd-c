#ifndef FUSE_H
#define FUSE_H

/*
 * set of hosts and mount nodes
 * contains default owner and permissions
 */
struct ep_fuse_config {

};


int ep_fuse_init(struct ep_fuse_config *cfg);

#endif
