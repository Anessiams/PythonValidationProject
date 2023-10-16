#include "config.h"
#include "runner.h"
#include "outproxy.h"
#include "inproxy.h"

class Handler {
private:
    InProxy in_proxy;
    OutProxy out_proxy;
    ValidatorRunner runner;
public:
    explicit Handler(const Config &config);

    // performs an input/output cycle for the handler - taking input and returning an output
    std::string handle_io(const std::string &input);

    // cleans up all shared resources owned by the handler
    void cleanup_resources();
};