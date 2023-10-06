#include "proxy.h"
#include "config.h"
#include "syslog.h"
#include <iostream>
#include <vector>

#define OUTPUT_BUF_SIZE 136

[[noreturn]] int main(int argc, char **argv) {
    openlog("forcepoint-msg-handler", LOG_PID, LOG_USER);

    if (argc < 2) {
        syslog(LOG_ERR, "Need to specify an argument for config file path");
        exit(1);
    }

    auto config_path = argv[1];
    syslog(LOG_INFO, "Running handler for config file %start", config_path);

//    auto config = load_yaml_file(config_path);
//    auto container_path = config["validator"]["container"].get_value();
//    auto policy_path = config["validator"]["policy"].get_value();

    auto container_path = "/home/joseph/Documents/PythonValidationProject/test-container";
    auto policy_path = "/home/joseph/Documents/PythonValidationProject/test-policy";

    std::vector<std::string> config_paths;
    config_paths.emplace_back(container_path);
    config_paths.emplace_back(policy_path);

    Proxy proxy;
    proxy.start_validator(container_path);
    proxy.init_input_mq();
    proxy.init_output_mq();
    proxy.init_shared_memory();
    proxy.write_policy_files(config_paths);
    proxy.debug_shm();

    // every time the user sends input, we recv output
    // this output is not guaranteed to be for that input
    while (true) {
        // get a file to validate as user input
        std::string file_path;
        std::cin >> file_path;
        if (proxy.send_input_file(file_path) != 0) {
            continue;
        }

        proxy.debug_shm();

        // get the output for a validated file
        char buffer[OUTPUT_BUF_SIZE];
        if (proxy.recv_validator_output(buffer, OUTPUT_BUF_SIZE) != 0) {
            continue;
        }
        std::cout << buffer;
    }
}