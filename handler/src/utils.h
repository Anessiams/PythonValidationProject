#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>
#include <vector>
#include <limits>
#include <istream>

#define PATTERN " \t\f\v\n\r"

inline std::string trim(const std::string &str) {
    std::string s = str;
    s.erase(0, s.find_first_not_of(PATTERN));
    s.erase(s.find_last_not_of(PATTERN) + 1);
    return s;
}

inline void find_and_replace(std::string &str, char find, char replace) {
    for (char &i : str) {
        if (i == find) {
            i = replace;
        }
    }
}

inline std::streamsize stream_size(std::istream &is) {
    is.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize length = is.gcount();
    is.clear(); // clears eof
    is.seekg(0, std::ios_base::beg);
    return length;
}

inline void expectToBe(const std::string& actual, const std::string& expected) {
    if (actual != expected) {
        printf("\nExpected %s, got %s", expected.c_str(), actual.c_str());
        exit(1);
    }
}

#endif