#include "runner.h"

ValidatorRunner::ValidatorRunner() = default;

void ValidatorRunner::run_one() {

}

void ValidatorRunner::run_many(int count) {
    for (int i = 0; i < count; i++) {
        this->run_one();
    }
}

void ValidatorRunner::stop_all() {

}
