#ifndef INPROXY_H_
#define INPROXY_H_

#include <string>
#include <mqueue.h>
#include <sys/mman.h>
#include <vector>
#include <memory>
#include "metadata.h"
#include "manager.h"

#define INPUT_MQ_NAME "/filesunvalidated"
#define SHM_NAME "/file-data"
#define BUF_SIZE 4096
#define SHM_SIZE 4096000

// a proxy class for the handler to communicate with input resources
class InProxy {
public:
    explicit InProxy(const std::vector<std::string> &policy_paths);

    ~InProxy();

    // writes an input file to the pool of shared memory using the metadata argument, returns 0 if successful
    // when the md size is different from the actual file size, the smaller value is used
    int write_file(std::ifstream &, const FileMetadata &);

    // sends an input file to the mq, returns 0 if successful
    int send_input_file(const std::string &);

    // informs that caller is done using an input file so resources can be freed
    void cleanup_input_file(const std::string &name);

    // debug shm by logging contents of policy metadata headers and files to syslog
    void debug_shm() const;
private:
    // writes the policy files into the policy section of shm, stops the process if this fails (we cannot handle that case)
    void write_policy_files(const std::vector<std::string> &paths);

    // input queue
    const struct mq_attr in_attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 128,
        .mq_curmsgs = 0,
    };
    mqd_t input_mq;
    // shared memory
    int shm_fd = 0;
    char *shm_ptr = nullptr;
    off_t inf_offset = 0; // offset where input files begin (after policy files)
    std::unique_ptr<FileManager> manager; // the file manager for the input files in shm
};

#endif