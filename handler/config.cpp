#include <config.h>
#include <fstream>
#include <syslog.h>
#include <utils.h>

YamlTree::YamlTree(const std::string &key, const std::string &value) {
    this->key = key;
    this->value = value;
}

YamlTree &YamlTree::operator[](const std::string &child_key) {
    for (std::unique_ptr<YamlTree> &child: this->children) {
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

    // scan lines until one of them is valid
    bool scan = true;
    std::string line;
    while(scan) {
        getline(is, line);
        std::string tr_line = trim(line);

        // keep looking if this line isn't parseable into a yaml node
        scan = tr_line.length() == 0 || tr_line[0] == '#';
    }

    auto delim = line.find(':');
    auto key = trim(line.substr(0, delim));
    auto value = trim(line.substr(delim + 1));

    auto tree = std::make_unique<YamlTree>(key, value);

    // TODO IMPLEMENT A DEPTH FIRST YAML PARSER

    return tree;
}

YamlTree &&load_yaml_file(const std::string &path) {
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
    return std::move(*tree);
}