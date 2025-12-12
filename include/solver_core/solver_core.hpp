#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <optional>
#include <functional>

namespace solver_core {

using number = long double;

struct Options {
    bool useEnhanced = true;
    bool useUltra = false;
    number target = 0.999999L;
    std::size_t threads = 0;
    std::chrono::seconds timeLimit{0};
};

struct Metrics {
    double duration_seconds = 0.0;
    std::size_t threads = 0;
};

struct Result {
    std::string formula;
    double score = 0.0;
    Metrics metrics;

    std::string to_json() const;
};

using InputMatrix = std::vector<std::vector<number>>;

class Context {
public:
    explicit Context(Options opts = {});
    Result run(const InputMatrix& input, const InputMatrix& expected);

    using JobId = std::string;
    using ProgressCallback = std::function<void(double)>;

    // Async API
    JobId runAsync(const InputMatrix& input, const InputMatrix& expected);
    std::optional<Result> pollResult(const JobId& id);
    void cancel(const JobId& id);
    void setProgressCallback(ProgressCallback cb);

    Metrics metrics() const;
private:
    Options opts_;
};

} 