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

        // Extract key and value from the line
        size_t delimiter_pos = tr_line.find(':');
        if (delimiter_pos == std::string::npos) {
            // Handle error - malformed YAML line
            continue;
        }
        std::string node_key = trim(tr_line.substr(0, delimiter_pos));
        std::string node_value = trim(tr_line.substr(delimiter_pos + 1));

        auto node = std::make_unique<YamlTree>(node_key, node_value);


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
    config.container_path = (*tree)["validator"]["container"].get_value();
    syslog(LOG_INFO, "Container path in config %s", config.container_path.c_str());

    std::string policy_files;
    auto &policy_files_tree = (*tree)["validator"]["policies"];
    auto &policy_files_children  = policy_files_tree.get_children();
    for (auto &p : policy_files_children) {
        config.policy_paths.push_back(p->get_value());
        policy_files += p->get_value() + " ";
    }
    syslog(LOG_INFO, "Policy path in config %s", policy_files.c_str());

    return config;
}