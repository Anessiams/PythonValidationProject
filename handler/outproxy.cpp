#include <syslog.h>
#include <cstdlib>
#include <cerrno>
#include <string>
#include "outproxy.h"

OutProxy::OutProxy() {
    this->output_mq = mq_open(OUTPUT_MQ_NAME, O_RDONLY | O_CREAT, 0666, &this->out_attr);
    syslog(LOG_INFO, "Opened output mq %d with attr maxmsg %ld and msgsize %ld", this->output_mq, out_attr.mq_maxmsg, out_attr.mq_msgsize);
    if (this->output_mq < 0) {
        syslog(LOG_ERR, "Failed to open output message queue %d with error %d", this->output_mq, errno);
        exit(1);
    }
}

OutProxy::~OutProxy() = default;


int OutProxy::receive_output(std::string &output) const {
    auto msg_len = this->out_attr.mq_msgsize + 1;
    char msg[msg_len];
    std::fill(msg, msg + msg_len, 0);

    ssize_t recv_len = mq_receive(this->output_mq, msg, this->out_attr.mq_msgsize, nullptr);
    if (recv_len < 0) {
        syslog(LOG_ERR, "Failed to recv from output mq with error %d", errno);
        return 2;
    }

    output = msg;

    syslog(LOG_INFO, "Recv message of length %ld from output mq", recv_len);
    return 0;
}
