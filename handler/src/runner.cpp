#include <syslog.h>
#include "runner.h"

ValidatorRunner::ValidatorRunner(std::string &container_tag) : container_tag(container_tag) {
}

void ValidatorRunner::run_one() {
//    auto run_command = "docker run -d " + container_tag;
//    int status = system(run_command.c_str());
//    if (status != 0) {
//        syslog(LOG_ERR, "Unable to start the container");
//        exit(1);
//    } else {
//        syslog(LOG_ERR, "Started the container using command %s", run_command.c_str());
//    }
}

void ValidatorRunner::run_many(int count) {
    for (int i = 0; i < count; i++) {
        run_one();
    }
}

void ValidatorRunner::stop_all() {

}
