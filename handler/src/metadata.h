#include <cstdio>
#include <string>

// the size of a string in metadata - a max length string will not include a null terminator
#define MD_STR_SIZE 4096

// a struct to represent the metadata of a file
typedef struct FileMetadata {
    char name[MD_STR_SIZE] = {0};
    off_t offset = 0;
    off_t size = 0;
} FileMetadata;

inline std::string metadata_to_string(FileMetadata &md) {
    std::string input_msg = md.name;
    input_msg += "," + std::to_string(md.offset) + "," + std::to_string(md.size);
    return input_msg;
}