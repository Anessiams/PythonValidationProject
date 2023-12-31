#include "inproxy.h"
#include "utils.h"
#include <syslog.h>
#include <fstream>
#include <csignal>
#include <cstring>

InProxy::InProxy(const std::vector<std::string> &policy_paths) {
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
    // write policy files to the shared memory as part of initialization
    write_policy_files(policy_paths);
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

    // the stream we use to determine the size for manager allocation is the same one use for reading the file
    std::ifstream is(path);
    if (!is) {
        syslog(LOG_ERR, "Cannot open file at path %s", path.c_str());
        return 1;
    }

    FileMetadata md;
    strncpy(md.name, path.c_str(), MD_STR_SIZE);
    md.size = stream_size(is);

    auto avail_space = SHM_SIZE - beg_inf_offset;
    if (md.size > avail_space) {
        // cannot write a file that is larger than the avail space
        syslog(LOG_ERR, "Cannot write a file %s of that size %ld with only %ld space", path.c_str(), md.size, avail_space);
        return 1;
    } else if (curr_inf_offset + md.size > SHM_SIZE) {
        // wrap around back to the start when we run out of space
        curr_inf_offset = beg_inf_offset;
    }

    md.offset = curr_inf_offset;

    if (write_file(is, md) != 0) {
        return 1;
    }

    curr_inf_offset += md.size;

    // write the metadata as input to message queue using CSV format
    auto input_msg = metadata_to_string(md);
    auto status = mq_send(input_mq, input_msg.c_str(), input_msg.size(), 0);

    // format the message to be written to syslog for debugging and log it
    find_and_replace(input_msg, FLD_DL, ' ');
    if (status != 0) {
        syslog(LOG_ERR, "Failed to send message %s to input mq with error %d", input_msg.c_str(), errno);
        return 1;
    }
    syslog(LOG_INFO, "Sent a message %s to the input mq", input_msg.c_str());
    return 0;
}

void InProxy::write_policy_files(const std::vector<std::string> &paths) {
    // total size includes the md entry size for each entry and a marker for entry count
    auto md_count = (off_t) paths.size();
    auto md_total_size = (off_t) sizeof(md_count) + md_count * (off_t) sizeof(FileMetadata);

    // set some space to write the metadata to, advance offset to write after
    auto curr_offset = md_total_size;

    FileMetadata metadata[md_count];

    // copy the contents of each policy file into memory mapped file
    for (int i = 0; i < paths.size(); i++) {
        auto &path = paths[i];
        auto &md = metadata[i];

        std::ifstream is(path);
        if (!is) {
            syslog(LOG_ERR, "Cannot open file at path %s", path.c_str());
            exit(1);
        }

        strncpy(md.name, path.c_str(), MD_STR_SIZE);
        md.size = stream_size(is);
        md.offset = curr_offset;

        if (write_file(is, md) != 0) {
            exit(1);
        }

        curr_offset += md.size;
    }

    // copy metadata and size into the shm
    std::memcpy(shm_ptr, &md_count, sizeof(md_count));
    std::memcpy(shm_ptr + sizeof(md_count), metadata, sizeof(FileMetadata) * md_count);

    // set the inf offset at the end of the policy files
    beg_inf_offset = curr_inf_offset = curr_offset;
    syslog(LOG_INFO, "Set input file offset to %ld", beg_inf_offset);
}

int InProxy::write_file(std::ifstream &is, const FileMetadata &md) {
    syslog(LOG_INFO, "Writing filename %s of size %ld into shm at offset %ld", md.name, md.size, md.offset);

    auto curr_offset = md.offset;
    auto final_offset = md.offset + md.size;

    // copy the file from disk into shm buffer by buffer, stops when there's nothing to get from the file, or we've written the provided size
    while (!is.eof() && curr_offset < final_offset) {
        off_t start_offset = curr_offset;

        // read a full buffer or the remaining size of bytes in the file
        auto remaining_size = final_offset - curr_offset;
        auto read_size = remaining_size < BUF_SIZE ? remaining_size : BUF_SIZE;
        is.read(shm_ptr + curr_offset, read_size);

        if (is.fail() && !is.eof()) {
            syslog(LOG_ERR, "Failed to read buffer from file %s", md.name);
            return 1;
        }

        syslog(LOG_INFO, "Read a buffer with contents %s", shm_ptr + curr_offset);

        // advance the offset and metadata counts
        auto count = is.gcount();
        curr_offset += count;

        syslog(LOG_INFO, "Writing %ld bytes for filename %s into shm at offset %ld", count, md.name, start_offset);
    }

    return 0;
}

void InProxy::debug_shm() const {
    off_t md_count;
    std::memcpy(&md_count, shm_ptr, sizeof(md_count));

    std::string md_string;
    FileMetadata metadata[md_count];
    for (int i = 0; i < md_count; i++) {
        auto shm_md_ptr = shm_ptr + sizeof(off_t) + sizeof(FileMetadata) * i;
        std::memcpy(&metadata[i], shm_md_ptr, sizeof(FileMetadata));
        md_string += metadata_to_string(metadata[i]) + " ";
    }

    // format the message to be written to syslog for debugging and log it
    find_and_replace(md_string, FLD_DL, ' ');
    auto shm_md_ptr = shm_ptr + sizeof(off_t) + sizeof(FileMetadata) * md_count;
    syslog(LOG_INFO, "%ld %s %s", md_count, md_string.c_str(), shm_md_ptr);
}

