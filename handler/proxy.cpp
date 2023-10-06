#include <proxy.h>
#include <syslog.h>
#include <sys/mman.h>
#include <fstream>
#include <csignal>
#include <cstring>

Proxy::Proxy() {
    // input mq attr
    in_attr.mq_flags = 0;
    in_attr.mq_maxmsg = 10;
    in_attr.mq_msgsize = 4096;
    in_attr.mq_curmsgs = 0;
    // output mq attr
    out_attr.mq_flags = 0;
    out_attr.mq_maxmsg = 10;
    out_attr.mq_msgsize = 4096;
    out_attr.mq_curmsgs = 0;
}

void Proxy::init_validator(const std::string &) {

}

void Proxy::init_input_mq() {
    this->input_mq = mq_open(INPUT_MQ_NAME, O_WRONLY | O_CREAT, 0666, &this->in_attr);
    syslog(LOG_INFO, "Opened input mq %d with attr maxmsg %ld and msgsize %ld", this->input_mq, in_attr.mq_maxmsg, in_attr.mq_msgsize);
    if (this->input_mq < 0) {
        syslog(LOG_ERR, "Failed to open input message queue %d with error %d", this->input_mq, errno);
        exit(1);
    }
}

void Proxy::init_output_mq() {
    this->output_mq = mq_open(OUTPUT_MQ_NAME, O_RDONLY | O_CREAT, 0666, &this->out_attr);
    syslog(LOG_INFO, "Opened output mq %d with attr maxmsg %ld and msgsize %ld", this->output_mq, out_attr.mq_maxmsg, out_attr.mq_msgsize);
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
    // memory map the shm
    this->shm_ptr = (char *) mmap(nullptr, this->shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    syslog(LOG_INFO, "Memory mapped to shm pointer %p", this->shm_ptr);
    if (shm_ptr == (void *) -1) {
        syslog(LOG_ERR, "Failed to memory map the shared memory %d", errno);
        exit(1);
    }
}

void Proxy::send_input_file(const std::string &path) {
    FileMetadata md;
    this->write_file(path, md);

    // write the metadata as input to message queue using CSV format
    std::string input_msg = md.name;
    input_msg += "," + std::to_string(md.offset) + "," + std::to_string(md.size);

    if (mq_send(this->input_mq, input_msg.c_str(), path.size(), 0) != 0) {
        syslog(LOG_ERR, "Failed to send message '%s' to path queue", path.c_str());
    }
}

ssize_t Proxy::recv_validator_output(char *buffer) const {
    auto buf_size = sizeof(buffer);
    auto mq_size = this->out_attr.mq_msgsize;
    if (buf_size < mq_size) {
        syslog(LOG_ERR,
               "Cannot recv from output queue: buffer MD_ENTRY_SIZE %lu should be at least than mq MD_ENTRY_SIZE %ld",
               buf_size, mq_size);
        return -1;
    }
    ssize_t read = mq_receive(this->output_mq, buffer, buf_size, nullptr);
    if (read < 0) {
        syslog(LOG_ERR, "Failed to recv from output queue");
    } else if (read == mq_size) {
        syslog(LOG_ERR,
               "Recv message MD_ENTRY_SIZE %zd is the same as max mq MD_ENTRY_SIZE truncation may have occured", read);
    }
    return read;
}

void Proxy::write_file(const std::string &path, FileMetadata &md) {
    std::ifstream is(path);
    if (!is) {
        syslog(LOG_ERR, "Cannot open file at path %s", path.c_str());
        exit(1);
    }

    strncpy(md.name, path.c_str(), MD_STR_SIZE - 1);
    md.offset = this->shm_offset;

    // copy the file from disk into shm buffer by buffer
    while (!is.eof()) {
        is.read(this->shm_ptr + this->shm_offset, BUF_SIZE);

        if (is.fail() && !is.eof()) {
            syslog(LOG_ERR, "Failed to read buffer from a policy file");
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

        syslog(LOG_INFO, "Wrote a buffer for filename %s of size %ld to %ld into shm", md.name, count, this->shm_offset);
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
        write_file(path, md);
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