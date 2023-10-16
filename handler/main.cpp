#include <syslog.h>
#include <iostream>
#include <csignal>
#include "config.h"
#include "inproxy.h"
#include "outproxy.h"
#include "runner.h"

void on_signal(int s) {
    syslog(LOG_INFO, "Handler received a shutdown signal %d", s);
    if (mq_unlink(INPUT_MQ_NAME) != 0) {
        syslog(LOG_INFO, "Failed to unlink input queue %s", INPUT_MQ_NAME);
    }
    if (mq_unlink(OUTPUT_MQ_NAME)) {
        syslog(LOG_INFO, "Failed to unlink output queue %s", OUTPUT_MQ_NAME);
    }
    if (shm_unlink(SHM_NAME)) {
        syslog(LOG_INFO, "Failed to unlink shm %s" ,SHM_NAME);
    }
}

[[noreturn]] int main(int argc, char **argv) {
    openlog("forcepoint-msg-handler", LOG_PID, LOG_USER);

    signal(SIGINT, on_signal);
    signal(SIGTERM, on_signal);

    if (argc < 2) {
        syslog(LOG_ERR, "Need to specify an argument for config file path");
        exit(1);
    }

    auto config_path = argv[1];
    syslog(LOG_INFO, "Running handler for config file %s", config_path);

//    auto yaml_tree = load_yaml_file(config_path);
    auto config = get_config();

    // start the input loop to relay stdin and stdout to resources
    InProxy in_proxy;
    OutProxy out_proxy;
    ValidatorRunner runner;

    in_proxy.write_policy_files(config.policy_paths);
    runner.run_many(config.validator_count);
    in_proxy.debug_shm();

    while (true) {
        std::string input;
        std::cin >> input;

        auto in_status = in_proxy.send_input_file(input);
        if (in_status != 0) {
            continue;
        }

        std::string output;
        auto out_status = out_proxy.receive_output(output);
        if (out_status != 0) {
            continue;
        }

        std::cout << output;
    }
}