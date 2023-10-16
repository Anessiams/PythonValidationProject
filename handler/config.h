#include <vector>
#include <memory>
#include <string>

struct Config {
    std::vector<std::string> policy_paths;
    int validator_count;
};

class YamlTree {
private:
    std::vector<std::unique_ptr<YamlTree>> children;
    std::string key;
    std::string value;
public:
    YamlTree(const std::string &, const std::string &);

    // get the child at a given key, throwing an exception if not defined
    YamlTree &operator[](const std::string &);

    // get the key stored at this node
    std::string &get_key();

    // get the value stored at this node
    std::string &get_value();
};

// parses the config into a yaml tree
YamlTree &&load_yaml_file(const std::string &);

// converts the parsed yaml tree to a config struct
Config get_config();