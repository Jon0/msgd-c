#ifndef LIBUTIL_CHANNEL_H
#define LIBUTIL_CHANNEL_H


/*
 * a client which requires updates when events occur
 * can be set to ignore some update types
 */
struct msgu_channel {
    int epid;
    int subid;
    int event_mask;
};


#endif
