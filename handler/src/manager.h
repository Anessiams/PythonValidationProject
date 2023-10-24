
#include <string>
#include <unordered_map>
#include <vector>
#include "metadata.h"

struct FreeBlock {
    off_t left_offset;
    off_t right_offset;
} typedef FreeBlock;

typedef std::vector<FreeBlock>::iterator FreeBlockIterator;

// keeps track of the named files stored in another block of memory such as a shared memory
// doesn't actually write to the block of memory that this is used to keep track of, rather gives offsets used to calculate pointers
class FileManager {
private:
    off_t memory_size; // size of the memory the files are in
    std::unordered_map<std::string, FileMetadata> files; // the files currently in the memory
    std::vector<FreeBlock> free_blocks; // data structure to store the free_file space to reserve_file efficiently
public:
    explicit FileManager(off_t);

    // reserves an offset and size for a named file within the block of memory
    // actual writing to the offset and size must happen elsewhere
    // writes the metadata for a reserved file to the md argument and returns 0 for success 1 otherwise
    int reserve_file(const std::string &name, off_t size, FileMetadata &md);

    // frees a region used for a named file by releasing free_file space and reference to the metadata
    // the freed region will NOT be zeroed out, but the offset range may appear in a subsequent reserve_file call
    void free_file(const std::string &name);
};
