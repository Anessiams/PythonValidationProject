#include <string>
#include <mqueue.h>
#include <vector>

// names for the shared resources
#define INPUT_MQ_NAME "/dev/mqueue/file_paths_unvalidated"
#define OUTPUT_MQ_NAME "/dev/mqueue/validated_results"
#define SHM_NAME "/dev/shm/configs"
// the size of a string in metadata including null terminator
#define MD_STR_SIZE 256
// the number of bytes to be read from a file at a given time
#define BUF_SIZE 4096
// the number of bytes to add with each resize
#define RESIZE_ADD 409600

// a struct to represent the metadata of a file
typedef struct FileMetadata {
    char name[MD_STR_SIZE];
    off_t offset;
    off_t size;
} FileMetadata;

// a proxy class for the handler to communicate with shared resources
class Proxy {
private:
    struct mq_attr in_attr = {};
    struct mq_attr out_attr = {};
    int shm_fd = 0;
    char *shm_ptr = nullptr;
    off_t shm_size = 0;
    off_t shm_offset = 0;
    mqd_t input_mq = 0;
    mqd_t output_mq = 0;
public:
    Proxy();

    // inits a validator at the specified file path
    void init_validator(const std::string &);

    // starts the input message queue
    void init_input_mq();

    // starts the output message queue
    void init_output_mq();

    // creates a pool of shared memory with a list of config files
    void init_shared_memory();

    // resizes the pool of shared memory by adding size
    void resize(off_t);

    // writes an input file to the pool of shared memory and writes to the metadata argument
    // all file writes to shm should be done through this function
    void write_file(const std::string &path, FileMetadata &);

    // writes a list of configuration files into shared memory
    void write_policy_files(const std::vector<std::string> &);

    // sends an input file to the mq
    void send_input_file(const std::string &);

    // receives output from the mq and puts it into the buffer, blocking if there is no output
    ssize_t recv_validator_output(char *) const;
};