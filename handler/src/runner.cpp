#include <syslog.h>
#include "runner.h"

ValidatorRunner::ValidatorRunner(std::string &container_path) : container_path(container_path) {
}

void ValidatorRunner::run_one() {
    auto run_command = "docker run -d " + container_path;
    int status = system(run_command.c_str());
    if (status == -1) {
        syslog(LOG_ERR, "Unable to start the container");
        exit(1);
    } else {
        syslog(LOG_ERR, "Started the the container using command %s", run_command.c_str());
    }
}

void ValidatorRunner::run_many(int count) {
    for (int i = 0; i < count; i++) {
        run_one();
    }
}

void ValidatorRunner::stop_all() {

}
