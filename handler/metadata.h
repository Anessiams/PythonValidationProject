#include <cstdio>
#include <string>

// the size of a string in metadata including null terminator
#define MD_STR_SIZE 256

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