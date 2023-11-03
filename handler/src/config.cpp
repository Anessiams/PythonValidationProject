#include "config.h"
#include <fstream>
#include <syslog.h>
#include "utils.h"
#include <stack>
#include <memory>
#include <utility>

YamlTree::YamlTree(std::string key, std::string value): key(std::move(key)), value(std::move(value)) {}

void YamlTree::add_child(std::unique_ptr<YamlTree> &child) {
    children.push_back(std::move(child));
}

YamlTree &YamlTree::operator[](const std::string &child_key) {
    for (auto &child: children) {
        if (child->get_key() == child_key) {
            return *child;
        }
    }
    syslog(LOG_ERR, "Cannot find child_key %s in yaml tree with child_key %s", child_key.c_str(), key.c_str());
    static YamlTree dummy("", "");
    return dummy;
}


std::string &YamlTree::get_key() {
    return key;
}

std::string &YamlTree::get_value() {
    return value;
}

std::vector<std::unique_ptr<YamlTree>> &YamlTree::get_children() {
    return children;
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
            nodes.top()->add_child(node);
            nodes.push(nodes.top()->get_children().back().get());
        } else {
            while (nodes.size() > indentation.size() / 2) {  // Assuming 2 spaces per level
                nodes.pop();
            }
            if (nodes.empty()) {
                tree->add_child(node);
                nodes.push(tree->get_children().back().get());
            } else {
                nodes.top()->add_child(node);
                nodes.push(nodes.top()->get_children().back().get());
            }
        }
        prevIndentation = indentation;
    }
    return tree;
}

std::unique_ptr<YamlTree> load_yaml_file(const std::string &path) {
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
    auto tree = load_yaml_file(path);

    Config config;
//    config.container_path = (*tree)["container"].get_value();
    config.policy_paths.emplace_back("../test-policy");
    config.policy_paths.emplace_back("../test-policy-1");
    return config;
}
