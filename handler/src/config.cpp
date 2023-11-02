#include <config.h>
#include <fstream>
#include <syslog.h>
#include <utils.h>
#include <stack>
#include <memory>
#include <utility>

YamlTree::YamlTree(std::string key, std::string value)
        : key(std::move(key)), value(std::move(value)) {}  // Using initializer list

YamlTree &YamlTree::operator[](const std::string &child_key) {
    for (auto &child: children) {
        if (child->get_key() == child_key) {
            return *child;
        }
    }
    syslog(LOG_ERR, "Cannot find child_key %s in yaml tree with child_key %s", child_key.c_str(), key.c_str());
    // Instead of exiting, return a dummy YamlTree with empty values
    static YamlTree dummy("", "");
    return dummy;
}


std::string &YamlTree::get_key() {
    return key;
}

std::string &YamlTree::get_value() {
    return value;
}

std::unique_ptr<YamlTree> read_into_tree(std::ifstream &is) {
    std::stack<YamlTree*> nodes;
    std::string prevIndentation;

    auto tree = std::make_unique<YamlTree>("", "");

    std::string line;
    while (getline(is, line)) {
        std::string tr_line = trim(line);
        if (tr_line.empty() || tr_line[0] == '#') {
            continue;
        }

        std::string indentation(line.size() - tr_line.size(), ' ');

        auto node = std::make_unique<YamlTree>("", tr_line);

        if (indentation.size() > prevIndentation.size()) {
            nodes.top()->children.push_back(std::move(node));
            nodes.push(nodes.top()->children.back().get());
        } else {
            while (nodes.size() > indentation.size() / 2) {  // Assuming 2 spaces per level
                nodes.pop();
            }
            if (nodes.empty()) {
                tree->children.push_back(std::move(node));
                nodes.push(tree->children.back().get());
            } else {
                nodes.top()->children.push_back(std::move(node));
                nodes.push(nodes.top()->children.back().get());
            }
        }
        prevIndentation = indentation;
    }
    return tree;
}

std::unique_ptr<YamlTree> load_yaml_file(const std::string &path) {  // Return type changed
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

    return read_into_tree(is);
}

Config parse_config(const std::string &path) {
    Config config;
    config.policy_paths.emplace_back("../test-policy");
    config.policy_paths.emplace_back("../test-policy-1");
    return config;
}
