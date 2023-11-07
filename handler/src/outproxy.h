#ifndef OUTPROXY_H_
#define OUTPROXY_H_

#include <cstdio>
#include <mqueue.h>

#define OUTPUT_MQ_NAME "/validatorresults"
#define OUT_DL '\037'

// a proxy class for the handler to communicate with output resources
class OutProxy {
public:
    OutProxy();

    ~OutProxy();

    // receives output from the mq and puts it into a string, blocking if there is no output, returns 0 if successful
    int receive_output(std::string &) const;

    // gets the name of the file from the output
    static std::string parse_name(std::string &output) ;
private:
    const struct mq_attr out_attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 128,
        .mq_curmsgs = 0,
    };
    mqd_t output_mq = 0;
};

#endif