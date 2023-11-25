#ifndef RUNNER_H_
#define RUNNER_H_

#include <string>
#include <vector>

class ValidatorRunner {
public:
    ValidatorRunner(std::vector<pid_t> &, std::mutex &, std::string &);

    // use_config a single validator
    void run_one();

    // use_config n validators
    void run_many(int);
private:
    std::string &container_tag;
    // a list containing all children of this process, the runner will write to whenever it forks an instance
    std::vector<pid_t> &pid_children;
    // a reference to the global lock protecting all pid_children for this process
    std::mutex &g_lock;
};

#endif