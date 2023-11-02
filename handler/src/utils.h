#include <string>

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