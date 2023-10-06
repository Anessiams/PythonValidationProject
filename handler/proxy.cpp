#include <proxy.h>
#include <syslog.h>
#include <sys/mman.h>
#include <fstream>
#include <csignal>
#include <cstring>

Proxy::Proxy() = default;

void Proxy::init_validator(const std::string &) {

}

void Proxy::init_input_mq() {
    this->input_mq = mq_open(INPUT_MQ_NAME, O_WRONLY | O_CREAT, 0666, &this->in_attr);
    syslog(LOG_INFO, "Opened input mq %d with attr maxmsg %ld and msgsize %ld", this->input_mq, in_attr.mq_maxmsg,
           in_attr.mq_msgsize);
    if (this->input_mq < 0) {
        syslog(LOG_ERR, "Failed to open input message queue %d with error %d", this->input_mq, errno);
        exit(1);
    }
}

void Proxy::init_output_mq() {
    this->output_mq = mq_open(OUTPUT_MQ_NAME, O_RDONLY | O_CREAT, 0666, &this->out_attr);
    syslog(LOG_INFO, "Opened output mq %d with attr maxmsg %ld and msgsize %ld", this->output_mq, out_attr.mq_maxmsg,
           out_attr.mq_msgsize);
    if (this->output_mq < 0) {
        syslog(LOG_ERR, "Failed to open output message queue %d with error %d", this->output_mq, errno);
        exit(1);
    }
}

void Proxy::init_shared_memory() {
    this->shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    syslog(LOG_INFO, "Opening shm %d", this->shm_fd);
    if (this->shm_fd < 0) {
        syslog(LOG_ERR, "Failed to open shared memory %d with error %d", this->shm_fd, errno);
        exit(1);
    }
    this->resize(4096);
    // memory map the shm
    this->shm_ptr = (char *) mmap(nullptr, this->shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    syslog(LOG_INFO, "Memory mapped to shm pointer %p", this->shm_ptr);
    if (shm_ptr == (void *) -1) {
        syslog(LOG_ERR, "Failed to memory map the shared memory %d", errno);
        exit(1);
    }
}

int Proxy::send_input_file(const std::string &path) {
    FileMetadata md;

    std::ifstream is(path);
    if (!is) {
        syslog(LOG_ERR, "Cannot open input file at path %s", path.c_str());
        return 1;
    }

    strncpy(md.name, path.c_str(), MD_STR_SIZE - 1);
    this->write_file(is, md);

    // write the metadata as input to message queue using CSV format
    std::string input_msg = md.name;
    input_msg += "," + std::to_string(md.offset) + "," + std::to_string(md.size);

    if (mq_send(this->input_mq, input_msg.c_str(), path.size(), 0) != 0) {
        syslog(LOG_ERR, "Failed to send message '%s' to path queue with error %d", input_msg.c_str(), errno);
    }
    syslog(LOG_ERR, "Sent an input file %s to the input message queue", path.c_str());
    return 0;
}

int Proxy::recv_validator_output(char *buffer, ssize_t buf_size) const {
    ssize_t msg_len = buf_size - 1;
    ssize_t recv_len = mq_receive(this->output_mq, buffer, msg_len, nullptr);
    if (recv_len < 0) {
        syslog(LOG_ERR, "Failed to recv from output queue with error %d", errno);
    }
    syslog(LOG_INFO, "Recv message of length %ld into buffer of size %ld", recv_len, msg_len);
    return 0;
}

void Proxy::write_file(std::ifstream &is, FileMetadata &md) {
    md.offset = this->shm_offset;

    // copy the file from disk into shm buffer by buffer
    while (!is.eof()) {
        is.read(this->shm_ptr + this->shm_offset, BUF_SIZE);

        if (is.fail() && !is.eof()) {
            syslog(LOG_ERR, "Failed to read buffer from file %s", md.name);
            exit(1);
        }

        // advance the offset and metadata counts
        auto count = is.gcount();
        this->shm_offset += count;
        md.size += count;

        // ensure we have enough space for more buffered reads
        if (this->shm_size - this->shm_offset < BUF_SIZE) {
            this->resize(RESIZE_ADD);
        }

        syslog(LOG_INFO, "Wrote a buffer for filename %s of size %ld to %ld into shm", md.name, count,
               this->shm_offset);
    }
}

void Proxy::write_policy_files(const std::vector<std::string> &paths) {
    // total size includes the md entry size for each entry and a marker for entry count
    auto md_count = (off_t) paths.size();
    auto md_total_size = (off_t) sizeof(md_count) + md_count * (off_t) sizeof(FileMetadata);

    // set some space to write the metadata to, advance offset to write after
    this->resize(md_total_size);
    this->shm_offset = md_total_size;

    FileMetadata metadata[md_count];

    // copy the contents of each policy file into memory mapped file
    for (int i = 0; i < paths.size(); i++) {
        auto &path = paths[i];
        auto &md = metadata[i];

        std::ifstream is(path);
        if (!is) {
            syslog(LOG_ERR, "Cannot open policy file at path %s", path.c_str());
            exit(1);
        }

        strncpy(md.name, path.c_str(), MD_STR_SIZE - 1);
        write_file(is, md);
    }

    // copy metadata into the shm
    std::memcpy(this->shm_ptr, &md_count, sizeof(md_count));
    std::memcpy(this->shm_ptr + sizeof(md_count), metadata, sizeof(FileMetadata) * md_count);
}

void Proxy::resize(off_t added_size) {
    auto new_size = this->shm_size + added_size;
    if (ftruncate(this->shm_fd, new_size) < 0) {
        syslog(LOG_ERR, "Failed to resize the memory mapped file to %ld", new_size);
        exit(1);
    }
    this->shm_size = new_size;
}