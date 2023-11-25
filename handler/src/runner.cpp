#include <syslog.h>
#include <csignal>
#include <mutex>
#include "runner.h"

ValidatorRunner::ValidatorRunner(std::vector<pid_t> &pid_children, std::mutex &g_lock, std::string &container_tag)
    : pid_children(pid_children), container_tag(container_tag), g_lock(g_lock) {
}

void ValidatorRunner::run_one() {
    pid_t pid = fork();
    if (pid == -1) {
        syslog(LOG_ERR, "Failed to fork a process for a validator instance");
        exit(1);
    }
    if (pid == 0) {
        // run a validator instance on the child process and exit once finished
        auto run_command = "docker run --ipc=host " + container_tag;
        int status = system(run_command.c_str());
        if (status != 0) {
            syslog(LOG_ERR, "Unable to start the container inside validator instance");
            exit(1);
        }
        syslog(LOG_INFO, "Started the a container in validator instance using command %s", run_command.c_str());
        exit(0);
    }
    g_lock.lock();
    pid_children.push_back(pid);
    g_lock.unlock();
}

void ValidatorRunner::run_many(int count) {
    for (int i = 0; i < count; i++) {
        run_one();
    }
}