#include <syslog.h>
#include "manager.h"

FileManager::FileManager(off_t memory_size) {
    this->memory_size = memory_size;
    auto fb = FreeBlock {
        .left_offset = 0,
        .right_offset = memory_size
    };
    free_blocks.emplace_back(fb);
}

int FileManager::reserve_file(const std::string &name, off_t size, FileMetadata &md) {
    if (files.count(name) > 0) {
        syslog(LOG_ERR, "Not enough shared memory to allocate a file to: no FreeBlocks");
        return 1;
    }

    // search for the first FreeBlock that can fit the file we want to allocate
    auto fb = free_blocks.begin();
    for (; fb != free_blocks.end() && fb->right_offset - fb->left_offset > size; fb++) {
    }
    // if we exhaust all elements (the iterator is at the end) we couldn't find a free block to use
    if (fb == free_blocks.end()) {
        syslog(LOG_ERR, "Not enough shared memory to allocate a file to: cannot find FreeBlock large enough to fit the file");
        return 1;
    }

    md.offset = fb->left_offset,
    md.size = size;
    name.copy(md.name, MD_STR_SIZE);
    files[name] = md;

    fb->left_offset += size;

    // remove a fb if it no longer has any space left
    if(fb->right_offset - fb->left_offset == 0) {
        free_blocks.erase(fb);
    }

    return 0;
}

void FileManager::free_file(const std::string &name) {
    auto md = files[name];
    files.erase(name);

    auto new_fb = FreeBlock {
        .left_offset = md.offset,
        .right_offset = md.offset + md.size
    };

    // if there are no free blocks at all, we just create a new one and stop
    if (free_blocks.empty()) {
        free_blocks.emplace_back(new_fb);
        return;
    }

    auto fb = free_blocks.begin();
    for (; fb != free_blocks.end(); fb++) {
        if (fb->left_offset == new_fb.right_offset) {
            // found a free block to the right that can merged with the new block (it starts where the new one ends)
            fb->left_offset -= md.size;
            break;
        } else if (fb->left_offset > new_fb.right_offset) {
            // found the block the new block must be before (but there is space between them)
            free_blocks.insert(fb, new_fb);
            break;
        }
    }
    // attempt to merge the free block with the one to the left if it can be done
    if (fb != free_blocks.begin()) {
        auto left_fb = fb - 1;
        if (fb->left_offset == left_fb->right_offset) {
            // merge the left free block to this one
            fb->left_offset -= left_fb->right_offset - left_fb->left_offset;
            free_blocks.erase(left_fb);
        }
    }
}
