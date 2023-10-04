#include "proxy.h"
#include "config.h"
#include "syslog.h"
#include <iostream>
#include <vector>

#define OUTPUT_BUF_SIZE 128

int main(int argc, char **argv) {
    openlog("forcepoint-msg-handler", LOG_PID, LOG_USER);

    if (argc < 2) {
        syslog(LOG_ERR, "Need to specify an argument for config file path");
        exit(1);
    }

    auto config_path = argv[1];
    syslog(LOG_INFO, "Running handler for config file %s", config_path);

    auto config = load_yaml_file(config_path);
    auto container_path = config["validator"]["container"].get_value();
    auto policy_path = config["validator"]["policy"].get_value();

    std::vector<std::string> config_paths;
    config_paths.push_back(container_path);
    config_paths.push_back(policy_path);

    Proxy proxy;
    proxy.init_validator(container_path);
    proxy.init_input_mq();
    proxy.init_output_mq();
    proxy.init_shared_memory();
    proxy.write_policy_files(config_paths);

    // every time the user sends input, we recv output
    // this output is not guaranteed to be for that input
    for (;;) {
        // get a file to validate as user input
        std::string file_path;
        std::cin >> file_path;
        proxy.send_input_file(file_path);

        // get the output for a validated file
        char buffer[OUTPUT_BUF_SIZE];
        auto size_read = proxy.recv_validator_output(buffer);
        if (size_read < 0) {
            continue;
        }
        if (size_read < OUTPUT_BUF_SIZE) {
            buffer[size_read] = '\0';
        }
        std::cout << buffer;
    }
}