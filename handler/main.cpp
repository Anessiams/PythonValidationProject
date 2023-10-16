#include <syslog.h>
#include <iostream>
#include <fstream>
#include <atomic>
#include <csignal>
#include <thread>
#include "handler.h"

std::atomic<bool> interrupted(false);

void interrupt(int) {
    syslog(LOG_INFO, "Handler signalled to interrupt");
    interrupted.store(true);
}

[[noreturn]] int main(int argc, char **argv) {
    signal(SIGINT, interrupt);
    signal(SIGTERM, interrupt);

    openlog("forcepoint-msg-handler", LOG_PID, LOG_USER);

    if (argc < 2) {
        syslog(LOG_ERR, "Need to specify an argument for config file path");
        exit(1);
    }

    auto config_path = argv[1];
    syslog(LOG_INFO, "Running handler for config file %s", config_path);

//    auto yaml_tree = load_yaml_file(config_path);
    auto config = get_config();

    Handler handler(config);

    // start a thread to wait for an interrupt then cleanup the handler
    auto wait_cleanup = [](Handler *handler) {
        while(!interrupted.load()) {
        }
        syslog(LOG_INFO, "Handler received an interruption");
        handler->cleanup_resources();
    };
    std::thread cleanup_thread(wait_cleanup, &handler);

    // start the input loop to relay stdin and stdout to handler's io
    while (true) {
        std::string input;
        std::cin >> input;
        std::string output = handler.handle_io(input);
        std::cout << output;
    }
}