#include <string>

class ValidatorRunner {
    std::string &container_path;
public:
    explicit ValidatorRunner(std::string &);

    // use_config a single validator
    void run_one();

    // use_config n validators
    void run_many(int);

    // stop all validator processes and instruct them to clean-up
    void stop_all();
};
