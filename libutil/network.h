#ifndef LIBUTIL_NETWORK_H
#define LIBUTIL_NETWORK_H


/*
 * defines attributes of a connection
 */
struct msgu_link {

};


/*
 * a set of hosts
 */
struct msgu_network {
    int network_id;
};


void msgu_any_request(struct msgu_link *l);


#endif
