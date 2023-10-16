#include <string>
#include <mqueue.h>
#include <sys/mman.h>
#include <vector>
#include "metadata.h"

// names for the shared resources
#define INPUT_MQ_NAME "/filesunvalidated"
#define SHM_NAME "/file-data"
// the number of bytes to be read from a file at a given time
#define BUF_SIZE 4096
// the size of the shm - 4mb
#define SHM_SIZE 4096000

// a proxy class for the handler to communicate with input resources
class InProxy {
private:
    // input queue
    const struct mq_attr in_attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 4096,
        .mq_curmsgs = 0,
    };
    mqd_t input_mq;
    // shm for input files
    int shm_fd = 0;
    char *shm_ptr = nullptr;
    off_t curr_offset = 0; // current write offset to shm
    off_t inf_offset = 0; // offset where input files begin (after policy files)
public:
    InProxy();

    ~InProxy();

    // writes the policy files into the policy section of shm
    void write_policy_files(const std::vector<std::string> &paths);

    // writes an input file to the pool of shared memory at the current offset and sets the metadata argument
    void write_file(std::ifstream &is, FileMetadata &);

    // sends an input file to the mq, returns 0 if successful
    int send_input_file(const std::string &);

    // gets a pointer to the metadata in shm at the index
    inline char *get_md_ptr(off_t md_index) const;

    // debug shm by logging contents of policy metadata headers and files to syslog
    void debug_shm() const;
};