#include <string>
#include <mqueue.h>
#include <vector>

#define INPUT_MQ_NAME "/dev/mqueue/file_paths_unvalidated"
#define OUTPUT_MQ_NAME "/dev/mqueue/validated_results"
#define SHM_NAME "/dev/shm/configs"
#define MD_STR_SIZE 256

// a struct to represent the metadata of a file
typedef struct FileMetadata {
    char name[MD_STR_SIZE];
    off_t offset;
    off_t size;
} FileMetadata;

constexpr off_t MD_COUNT_SIZE = sizeof(int);
constexpr off_t MD_ENTRY_SIZE = sizeof(FileMetadata::name) + sizeof(FileMetadata::offset) + sizeof(FileMetadata::size);

// a proxy class for the handler to communicate with shared resources
class Proxy {
private:
    struct mq_attr in_attr = {};
    struct mq_attr out_attr = {};
    int shm_fd = 0;
    void *shm_ptr = nullptr;
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

    // resizes the pool of shared memory by adding MD_ENTRY_SIZE
    void resize(off_t);

    // writes a list of configuration files into shared memory
    void write_policy_files(const std::vector<std::string> &);

    // writes a file to the pool of shared memory
    FileMetadata write_file(const std::string &);

    // sends an input file to the mq
    void send_input_file(const std::string &);

    // receives output from the mq and puts it into the buffer, blocking if there is no output
    ssize_t recv_validator_output(char *) const;
};