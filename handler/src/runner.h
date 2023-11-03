#ifndef RUNNER_H_
#define RUNNER_H_

#include <string>

class ValidatorRunner {
public:
    explicit ValidatorRunner(std::string &);

    // use_config a single validator
    void run_one();

    // use_config n validators
    void run_many(int);

    // stop all validator processes and instruct them to clean-up
    void stop_all();
private:
    std::string &container_path;
};

#endif