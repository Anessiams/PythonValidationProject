#include "manager.h"
#include "utils.h"
#include <cstring>

std::string output_md(FileMetadata &md) {
    auto str = metadata_to_string(md);
    find_and_replace(str, FLD_DL, ' ');
    printf("%s", str.c_str());
    return str;
}

void testReserve() {
    FileManager manager(128, 512);

    printf("\n");
    std::string out;

    FileMetadata md;
    strncpy(md.name, "test1", MD_STR_SIZE);
    md.size = 100;
    manager.reserve_file(md);
    out += output_md(md);

    FileMetadata md1;
    strncpy(md1.name, "test2", MD_STR_SIZE);
    md1.size = 120;
    manager.reserve_file(md1);
    out += output_md(md1);

    expectToBe(out, "test1 128 100test2 228 120");
}

void testReserveOverflow() {
    FileManager manager(128, 512);

    printf("\n");

    FileMetadata md;
    strncpy(md.name, "test1", MD_STR_SIZE);
    md.size = 100;
    manager.reserve_file(md);
    output_md(md);

    FileMetadata md1;
    strncpy(md1.name, "test2", MD_STR_SIZE);
    md1.size = 900;
    auto out = manager.reserve_file(md1);
    output_md(md1);

    expectToBe(std::to_string(out), "1");
}

void testReserveThenFree() {
    FileManager manager(128, 512);

    printf("\n");
    std::string out;

    FileMetadata md;
    strncpy(md.name, "test1", MD_STR_SIZE);
    md.size = 100;
    manager.reserve_file(md);
    out += output_md(md);

    FileMetadata md1;
    strncpy(md1.name, "test2", MD_STR_SIZE);
    md1.size = 120;
    manager.reserve_file(md1);
    out += output_md(md1);

    manager.free_file("test1");

    FileMetadata md2;
    strncpy(md2.name, "test3", MD_STR_SIZE);
    md2.size = 90;
    manager.reserve_file(md2);
    out += output_md(md2);

    expectToBe(out, "test1 128 100test2 228 120test3 128 90");
}

int main() {
    testReserve();
    testReserveOverflow();
    testReserveThenFree();
    printf("\nPassed all tests successfully");
}