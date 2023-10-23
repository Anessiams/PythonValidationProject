#include "runner.h"

ValidatorRunner::ValidatorRunner(std::string &container_path) : container_path(container_path) {
}

void ValidatorRunner::run_one() {

}

void ValidatorRunner::run_many(int count) {
    for (int i = 0; i < count; i++) {
        run_one();
    }
}

void ValidatorRunner::stop_all() {

}
