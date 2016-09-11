#ifndef ROUTE_H
#define ROUTE_H


/*
 * each process recieves updates from nodes
 */
struct msg_subscriber {
    int host;
    int nodes_ids [32];
};

/*
 * return true or false
 */
int subroute(const char *tmpl, const char *route);


#endif
