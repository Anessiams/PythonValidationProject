#include <proxy.h>
#include <syslog.h>
#include <sys/mman.h>

Proxy::Proxy() {
    // input mq attr
    in_attr.mq_flags = 0;
    in_attr.mq_maxmsg = 100;
    in_attr.mq_msgsize = 4096;
    in_attr.mq_curmsgs = 0;
    // output mq attr
    out_attr.mq_flags = 0;
    out_attr.mq_maxmsg = 100;
    out_attr.mq_msgsize = 4096;
    out_attr.mq_curmsgs = 0;
}

void Proxy::init_validator(const std::string &) {

}

void Proxy::init_input_mq() {
    this->input_mq = mq_open(INPUT_MQ_NAME, O_WRONLY | O_CREAT, 0666, &this->in_attr);
    if (this->input_mq < 0) {
        syslog(LOG_ERR, "Failed to open input message queue");
        exit(1);
    }
}

void Proxy::init_output_mq() {
    this->output_mq = mq_open(OUTPUT_MQ_NAME, O_RDONLY | O_CREAT, 0666, &this->out_attr);
    if (this->output_mq < 0) {
        syslog(LOG_ERR, "Failed to open output message queue");
        exit(1);
    }
}

void Proxy::init_shared_memory() {
    this->shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    if (this->shm_fd < 0) {
        syslog(LOG_ERR, "Failed to open shared memory");
        exit(1);
    }
    // memory map the shm
    this->shm_ptr = mmap(NULL, this->shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == (void *) -1) {
        syslog(LOG_ERR, "Failed to memory map the shared memory");
        exit(1);
    }
}

void Proxy::send_input_file(const std::string &input) const {
    if (mq_send(this->input_mq, input.c_str(), input.size(), 0) != 0) {
        syslog(LOG_ERR, "Failed to send message '%s' to input queue", input.c_str());
    }
}

ssize_t Proxy::recv_validator_output(char* buffer) const {
    auto buf_size = sizeof(buffer);
    auto mq_size = this->out_attr.mq_msgsize;
    if (buf_size < mq_size) {
        syslog(LOG_ERR, "Cannot recv from output queue: buffer size %lu should be at least than mq size %ld", buf_size, mq_size);
        return -1;
    }
    ssize_t read = mq_receive(this->output_mq, buffer, buf_size, nullptr);
    if (read < 0) {
        syslog(LOG_ERR, "Failed to recv from output queue");
    } else if (read == mq_size) {
        syslog(LOG_ERR, "Recv message size %zd is the same as max mq size truncation may have occured", read);
    }
    return read;
}

void Proxy::write_configs(const std::vector<std::string>&) {

}


void Proxy::write_buffer(char *buffer, ssize_t size) {

}

void Proxy::write_file(const std::string &) {

}