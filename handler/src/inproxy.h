#ifndef INPROXY_H_
#define INPROXY_H_

#include <string>
#include <mqueue.h>
#include <sys/mman.h>
#include <vector>
#include "metadata.h"

#define INPUT_MQ_NAME "/filesunvalidated"
#define SHM_NAME "/file-data"
#define BUF_SIZE 4096
#define SHM_SIZE 4096000

// a proxy class for the handler to communicate with input resources
class InProxy {
public:
    explicit InProxy(const std::vector<std::string> &paths);

    ~InProxy();

    // writes an input file to the pool of shared memory at the current offset and sets the metadata argument
    int write_file(FileMetadata &);

    // sends an input file to the mq, returns 0 if successful
    int send_input_file(const std::string &);

    // debug shm by logging contents of policy metadata headers and files to syslog
    void debug_shm() const;
private:
    // writes the policy files into the policy section of shm
    void write_policy_files(const std::vector<std::string> &paths);

    const struct mq_attr in_attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 128,
        .mq_curmsgs = 0,
    };
    mqd_t input_mq;
    int shm_fd = 0;
    char *shm_ptr = nullptr;
    off_t inf_offset = 0; // offset where input files begin (after policy files)
    off_t curr_offset = 0; // current write offset to shm
};

#endif