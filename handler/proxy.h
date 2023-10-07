#include <string>
#include <mqueue.h>
#include <vector>

// names for the shared resources
#define INPUT_MQ_NAME "/filesunvalidated"
#define OUTPUT_MQ_NAME "/validatorresults"
#define SHM_NAME "/file-data"
// the size of a string in metadata including null terminator
#define MD_STR_SIZE 256
// the number of bytes to be read from a file at a given time
#define BUF_SIZE 4096
// the size of the shm - 4mb
#define SHM_SIZE 4096000

// a struct to represent the metadata of a file
typedef struct FileMetadata {
    char name[MD_STR_SIZE];
    off_t offset;
    off_t size;
} FileMetadata;

inline std::string metadata_to_string(FileMetadata &md) {
    std::string input_msg = md.name;
    input_msg += "," + std::to_string(md.offset) + "," + std::to_string(md.size);
    return input_msg;
}

// a proxy class for the handler to communicate with shared resources
class Proxy {
private:
    struct mq_attr in_attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 4096,
        .mq_curmsgs = 0,
    };
    struct mq_attr out_attr = {
        .mq_flags = 0,
        .mq_maxmsg = 10,
        .mq_msgsize = 128,
        .mq_curmsgs = 0,
    };
    int shm_fd = 0;
    char *shm_ptr = nullptr;
    off_t curr_offset = 0; // current write offset to shm
    off_t inf_offset = 0; // offset where input files begin (after policy files)
    mqd_t input_mq = 0;
    mqd_t output_mq = 0;
public:
    Proxy();

    // inits a validator at the specified file path
    void start_validator(const std::string &);

    // starts the input message queue
    void init_input_mq();

    // starts the output message queue
    void init_output_mq();

    // creates a pool of shared memory with a list of config files
    void init_shared_memory();

    // writes the policy files into the policy section of shm
    void write_policy_files(const std::vector<std::string> &paths);

    // writes an input file to the pool of shared memory at the current offset and sets the metadata argument
    void write_file(std::ifstream &is, FileMetadata &);

    // sends an input file to the mq, returns 0 if successful
    int send_input_file(const std::string &);

    // receives output from the mq and puts it into a buffer, blocking if there is no output, returns 0 if successful
    int recv_validator_output(char *, ssize_t) const;

    // debug shm by logging contents of policy metadata headers and files to syslog
    void debug_shm();
};