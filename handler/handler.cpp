#include <csignal>
#include <iostream>
#include <sys/syslog.h>
#include "handler.h"

Handler::Handler(const Config &config) {
    this->in_proxy.write_policy_files(config.policy_paths);
    runner.run_many(config.validator_count);
}

std::string Handler::handle_io(const std::string &input) {
    auto in_status = this->in_proxy.send_input_file(input);
    if (in_status != 0) {
        return "";
    }

    std::string output;
    auto out_status = this->out_proxy.receive_output(output);
    if (out_status != 0) {
        return "";
    }

    return output;
}

void Handler::cleanup_resources() {
    this->in_proxy.cleanup_resources();
    this->out_proxy.cleanup_resources();
    this->runner.stop_all();
    syslog(LOG_INFO, "Finished cleaning up handler's resources");
}
