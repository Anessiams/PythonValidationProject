#include <proxy.h>
#include <syslog.h>
#include <sys/mman.h>
#include <fstream>
#include <csignal>
#include <cstring>

#define ITH_MD_OFFSET(i) (this->shm_ptr + sizeof(md_count) + sizeof(FileMetadata) * i)

Proxy::Proxy() = default;

void Proxy::start_validator(const std::string &) {

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
    syslog(LOG_INFO, "Opened shm %d", this->shm_fd);
    if (this->shm_fd < 0) {
        syslog(LOG_ERR, "Failed to open shared memory %d with error %d", this->shm_fd, errno);
        exit(1);
    }
    // set the size for shm
    if (ftruncate(this->shm_fd, SHM_SIZE) < 0) {
        syslog(LOG_ERR, "Failed to set the shm size to %d", SHM_SIZE);
        exit(1);
    }
    // memory map the shm
    this->shm_ptr = (char *) mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == (void *) -1) {
        syslog(LOG_ERR, "Failed to memory map the shared memory %d", errno);
        exit(1);
    }
    syslog(LOG_INFO, "Memory mapped shm to address %p", this->shm_ptr);
}

int Proxy::send_input_file(const std::string &path) {
    FileMetadata md;

    std::ifstream is(path);
    if (!is) {
        syslog(LOG_ERR, "Cannot open input file at path %s", path.c_str());
        return 1;
    }

    strncpy(md.name, path.c_str(), MD_STR_SIZE - 1);
    this->curr_offset = this->inf_offset;
    this->write_file(is, md);

    // write the metadata as input to message queue using CSV format
    std::string input_msg = metadata_to_string(md);

    if (mq_send(this->input_mq, input_msg.c_str(), path.size(), 0) != 0) {
        syslog(LOG_ERR, "Failed to send message '%s' to input mq with error %d", input_msg.c_str(), errno);
        return 1;
    }
    syslog(LOG_ERR, "Sent an input file %s to the input mq", path.c_str());
    return 0;
}

int Proxy::recv_validator_output(char *buffer, ssize_t buf_size) const {
    ssize_t msg_len = buf_size - 1;
    ssize_t recv_len = mq_receive(this->output_mq, buffer, msg_len, nullptr);
    if (recv_len < 0) {
        syslog(LOG_ERR, "Failed to recv from output mq with error %d", errno);
        return 1;
    }
    syslog(LOG_INFO, "Recv message of length %ld into buffer of size %ld from output mq", recv_len, msg_len);
    return 0;
}

void Proxy::write_policy_files(const std::vector<std::string> &paths) {
    // total size includes the md entry size for each entry and a marker for entry count
    auto md_count = (off_t) paths.size();
    auto md_total_size = (off_t) sizeof(md_count) + md_count * (off_t) sizeof(FileMetadata);

    // set some space to write the metadata to, advance offset to write after
    this->curr_offset = md_total_size;

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

    // copy metadata and size into the shm
    std::memcpy(this->shm_ptr, &md_count, sizeof(md_count));
    std::memcpy(this->shm_ptr + sizeof(md_count), metadata, sizeof(FileMetadata) * md_count);

    // set the inf offset at the end of the policy files
    this->inf_offset = this->curr_offset;
}

void Proxy::write_file(std::ifstream &is, FileMetadata &md) {
    md.offset = this->curr_offset;

    // copy the file from disk into shm buffer by buffer
    while (!is.eof()) {
        is.read(this->shm_ptr + this->curr_offset, BUF_SIZE);

        if (is.fail() && !is.eof()) {
            syslog(LOG_ERR, "Failed to read buffer from file %s", md.name);
            exit(1);
        }

        // advance the offset and metadata counts
        auto count = is.gcount();
        this->curr_offset += count;
        md.size += count;

        syslog(LOG_INFO, "Writing %ld bytes for filename %s into shm at offset %ld", count, md.name, this->curr_offset);
    }
}

void Proxy::debug_shm() {
    off_t md_count;
    std::memcpy(&md_count, this->shm_ptr, sizeof(md_count));

    std::string md_log;
    FileMetadata metadata[md_count];
    for (int i = 0; i < md_count; i++) {
        std::memcpy(&metadata[i], ITH_MD_OFFSET(i), sizeof(FileMetadata));
        md_log += metadata_to_string(metadata[i]) + " ";
    }

    syslog(LOG_INFO, "%ld %s %s", md_count, md_log.c_str(), ITH_MD_OFFSET(md_count));
}
