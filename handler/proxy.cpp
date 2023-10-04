#include <proxy.h>
#include <syslog.h>
#include <sys/mman.h>
#include <fstream>
#include <csignal>

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
    this->shm_ptr = mmap(nullptr, this->shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == (void *) -1) {
        syslog(LOG_ERR, "Failed to memory map the shared memory");
        exit(1);
    }
}

void Proxy::send_input_file(const std::string &path) {
    auto md = this->write_file(path);

    // write the metadata as input to message queue so consumer knows a new file must be processed
    // TODO IMPLEMENT A INPUT MSG FORMAT

    if (mq_send(this->input_mq, path.c_str(), path.size(), 0) != 0) {
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

void Proxy::write_policy_files(const std::vector<std::string> &paths) {
    // total size includes the md entry size for each entry and a marker for entry count
    auto md_total_size = + MD_COUNT_SIZE + (off_t) paths.size() * MD_ENTRY_SIZE;

    // set some space to write the metadata to later
    this->resize(md_total_size);
    this->shm_offset = md_total_size;

    FileMetadata metadata[paths.size()];

    // copy the contents of each policy file into memory mapped file
    for (int i = 0; i < paths.size(); i++) {
        auto &path = paths[i];

        std::ifstream is(path);
        if (!is) {
            syslog(LOG_ERR, "Cannot open file at path %s", path.c_str());
            exit(1);
        }

        auto &md = metadata[i];
        // copy the file from disk into shm buffer by buffer and keep track of metadata
        // TODO IMPLEMENT BUFFERED FILE IO
    }

    // go back and write the metadata into the empty space we left
    this->shm_offset = 0;
    // TODO IMPLEMENT WRITE METADATA
}

void Proxy::resize(off_t size) {
    auto new_size = this->shm_size + size;
    if (ftruncate(this->shm_fd, new_size) < 0) {
        syslog(LOG_ERR, "Failed to resize the memory mapped file to %ld", new_size);
        exit(1);
    }
    this->shm_size = new_size;
}


FileMetadata Proxy::write_file(const std::string &path) {
    FileMetadata md;

    std::ifstream is(path);
    if (!is) {
        syslog(LOG_ERR, "Cannot open file at path %s", path.c_str());
        exit(1);
    }

    // copy the file from disk into shm buffer by buffer and keep track of metadata
    // TODO IMPLEMENT BUFFERED FILE IO

    return md;
}