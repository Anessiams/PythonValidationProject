#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>
#include <limits>
#include <istream>

#define PATTERN " \t\f\v\n\r"

inline std::string trim(std::string &str) {
    return str.erase(0, str.find_first_not_of(PATTERN))
        .erase(str.find_last_not_of(PATTERN));
}

inline std::string trim(std::string &&str) {
    return trim(str);
}

inline void find_and_replace(std::string &str, char find, char replace) {
    for (char &i : str) {
        if (i == find) {
            i = replace;
        }
    }
}

inline std::streamsize is_size(std::istream &is) {
    is.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize length = is.gcount();
    is.clear(); // clears eof
    is.seekg(0, std::ios_base::beg);
    return length;
}

#endif