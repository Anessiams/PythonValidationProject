#include <string>

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
