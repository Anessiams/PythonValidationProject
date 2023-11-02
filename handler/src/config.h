#include <vector>
#include <memory>
#include <string>

struct Config {
    std::vector<std::string> policy_paths;
    std::string container_path;
    int child_count = 1;
};

class YamlTree {
private:
    std::string key;
    std::string value;
public:
    YamlTree(std::string , std::string );

    // get the child at a given key, throwing an exception if not defined
    YamlTree &operator[](const std::string &);

    // get the key stored at this node
    std::string &get_key();

    // get the value stored at this node
    std::string &get_value();

    std::vector<std::unique_ptr<YamlTree>> children;
};

// parses the config file into a yaml tree
std::unique_ptr<YamlTree> load_yaml_file(const std::string &);

// converts the config file to a config struct
Config parse_config(const std::string &);