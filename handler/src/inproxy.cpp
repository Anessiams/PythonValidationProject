#include "inproxy.h"
#include <syslog.h>
#include <fstream>
#include <csignal>
#include <cstring>

InProxy::InProxy() {
    // opening up the input queue
    input_mq = mq_open(INPUT_MQ_NAME, O_WRONLY | O_CREAT, 0666, &in_attr);
    syslog(LOG_INFO, "Opened input mq with attr maxmsg %ld and msgsize %ld", in_attr.mq_maxmsg, in_attr.mq_msgsize);
    if (input_mq < 0) {
        syslog(LOG_ERR, "Failed to open input message queue with error %d", errno);
        exit(1);
    }
    // opening up the shm for input
    shm_fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0666);
    syslog(LOG_INFO, "Opened shm %d", shm_fd);
    if (shm_fd < 0) {
        syslog(LOG_ERR, "Failed to open shared memory with error %d", errno);
        exit(1);
    }
    // set the size for shm
    if (ftruncate(shm_fd, SHM_SIZE) < 0) {
        syslog(LOG_ERR, "Failed to set the shm size to %d", SHM_SIZE);
        exit(1);
    }
    // memory map the shm
    shm_ptr = (char *) mmap(nullptr, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == (void *) -1) {
        syslog(LOG_ERR, "Failed to memory map the shm with error %d", errno);
        exit(1);
    }
    syslog(LOG_INFO, "Memory mapped shm to address %p", shm_ptr);
}

InProxy::~InProxy() {
    // close input message queue
    if (mq_close(input_mq) != 0) {
        syslog(LOG_ERR, "Failed to close input message queue with error %d", errno);
    }
    // unmap the shm
    if (munmap(shm_ptr, SHM_SIZE) != 0) {
        syslog(LOG_ERR, "Failed to memory unmap the shm with error %d", errno);
    }
    // close the shm
    if (close(shm_fd) != 0) {
        syslog(LOG_ERR, "Failed to close shared memory with error %d", errno);
    }
}

int InProxy::send_input_file(const std::string &path) {
    if (path.length() > MD_STR_SIZE) {
        syslog(LOG_ERR, "Length for input file path %ld must be less than %d", path.length(), MD_STR_SIZE);
        return 1;
    }

    FileMetadata md;
    strncpy(md.name, path.c_str(), MD_STR_SIZE);

    std::ifstream is(path);
    if (!is) {
        syslog(LOG_ERR, "Cannot open input file at path %s", path.c_str());
        return 1;
    }

    write_file(is, md);

    // write the metadata as input to message queue using CSV format
    auto input_msg = metadata_to_string(md);
    auto status = mq_send(input_mq, input_msg.c_str(), path.size(), 0);
    if (status != 0) {
        syslog(LOG_ERR, "Failed to send message '%s' to input mq with error %d", input_msg.c_str(), errno);
        return 2;
    }
    syslog(LOG_ERR, "Sent a message %s to the input mq", input_msg.c_str());
    return 0;
}

void InProxy::write_policy_files(const std::vector<std::string> &paths) {
    // total size includes the md entry size for each entry and a marker for entry count
    auto md_count = (off_t) paths.size();
    auto md_total_size = (off_t) sizeof(md_count) + md_count * (off_t) sizeof(FileMetadata);

    // set some space to write the metadata to, advance offset to write after
    curr_offset = md_total_size;

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
    std::memcpy(shm_ptr, &md_count, sizeof(md_count));
    std::memcpy(shm_ptr + sizeof(md_count), metadata, sizeof(FileMetadata) * md_count);

    // set the inf offset at the end of the policy files
    inf_offset = curr_offset;
}

void InProxy::write_file(std::ifstream &is, FileMetadata &md) {
    md.offset = curr_offset;
    md.size = 0;

    // copy the file from disk into shm buffer by buffer
    while (!is.eof()) {
        off_t start_offset = curr_offset;
        is.read(shm_ptr + curr_offset, BUF_SIZE);

        if (is.fail() && !is.eof()) {
            syslog(LOG_ERR, "Failed to read buffer from file %s", md.name);
            exit(1);
        }

        // advance the offset and metadata counts
        auto count = is.gcount();
        curr_offset += count;
        md.size += count;

        syslog(LOG_INFO, "Writing %ld bytes for filename %s into shm at offset %ld", count, md.name, start_offset);
    }
}

char *InProxy::get_md_ptr(off_t md_index) const {
    return shm_ptr + sizeof(off_t) + sizeof(FileMetadata) * md_index;
}

void InProxy::debug_shm() const {
    off_t md_count;
    std::memcpy(&md_count, shm_ptr, sizeof(md_count));

    std::string md_log;
    FileMetadata metadata[md_count];
    for (int i = 0; i < md_count; i++) {
        std::memcpy(&metadata[i], get_md_ptr(i), sizeof(FileMetadata));
        md_log += metadata_to_string(metadata[i]) + " ";
    }

    syslog(LOG_INFO, "%ld %s %s", md_count, md_log.c_str(), get_md_ptr(md_count));
}

