#ifndef UTILS_H_
#define UTILS_H_

#include <string>
#include <vector>

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

inline std::vector<std::string> tokenize(std::string &str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    for (auto c : str) {
        if (c == delimiter) {
            tokens.emplace_back(token);
            token = "";
        } else {
            token += c;
        }
    }
    return tokens;
}

#endif