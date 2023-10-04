#include <config.h>
#include <fstream>
#include <syslog.h>
#include <cstdio>
#include <utils.h>

YamlTree::YamlTree(const std::string &key, const std::string &value) {
    this->key = key;
    this->value = value;
}

YamlTree &YamlTree::operator[](const std::string &child_key) {
    for (std::unique_ptr<YamlTree> &child : this->children) {
        if (child->get_key() == child_key) {
            return *child;
        }
    }
    syslog(LOG_ERR, "Cannot find child_key %s in yaml tree with child_key %s", child_key.c_str(), this->key.c_str());
    exit(1);
}

std::string &YamlTree::get_key() {
    return this->key;
}

std::string &YamlTree::get_value() {
    return this->value;
}

std::unique_ptr<YamlTree> read_into_tree(std::ifstream &is) {
    std::string line;
    getline(is, line);
    std::string tr_line = trim(line);

    if(tr_line.length() == 0 || tr_line[0] == '#') {

    }

    auto delim = line.find(':');
    auto key = line.substr(0, delim);
    auto value = line.substr(delim + 1);

    auto tree = std::make_unique<YamlTree>(key, value);

    return tree;
}

YamlTree load_yaml_file(const std::string &path) {
    std::ifstream is(path);

    if (!is) {
        syslog(LOG_ERR, "Cannot open yaml file with path %s", path.c_str());
        exit(1);
    }

    std::string line;
    getline(is, line);

    if (line != "---") {
        syslog(LOG_ERR, "A yaml document must begin with ---");
        exit(1);
    }

    auto tree = read_into_tree(is);
}