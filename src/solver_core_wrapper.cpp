#include "solver_core/solver_core.hpp"
#include "solver/solver.hpp"
#include <mutex>
#include <unordered_map>
#include <functional>
#include <thread>
#include <chrono>
#include <sstream>
#include <atomic>

namespace solver_core {

Context::Context(Options opts) {
    opts_ = opts;
}

Result Context::run(const InputMatrix& input, const InputMatrix& expected) {
    std::vector<Variable> vars;
    if (!input.empty()) {
        const auto cols = input[0].size();
        for (size_t i = 0; i < cols; ++i) vars.emplace_back("x" + std::to_string(i));
    }

    Solver solver(std::move(vars), input, expected, opts_.useEnhanced, opts_.useUltra, opts_.target, opts_.threads, opts_.timeLimit);
    solver.start();

    Result r;
    const auto hall = solver.hallOfFameCopy();
    if (!hall.empty()) {
        const auto best = *hall.rbegin();
        r.formula = best.getFormula().toString();
        r.score = static_cast<double>(best.getRate());
    }
    r.metrics.duration_seconds = solver.elapsed_seconds();
    r.metrics.threads = solver.threads();
    return r;
}

Metrics Context::metrics() const {
    return Metrics{};
}

struct JobState {
    std::optional<Result> result;
    std::atomic<bool> cancelled{false};
    std::atomic<bool> done{false};
    std::shared_ptr<std::atomic<bool>> cancelFlag;
};

static std::mutex jobsMutex;
static std::unordered_map<std::string, std::shared_ptr<JobState>> jobs;
static std::function<void(double)> globalProgressCb = nullptr;

Context::JobId Context::runAsync(const InputMatrix& input, const InputMatrix& expected) {
    auto jobId = std::to_string(reinterpret_cast<uintptr_t>(std::hash<std::thread::id>()(std::this_thread::get_id()))) + "-" + std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
    auto state = std::make_shared<JobState>();
    {
        std::lock_guard<std::mutex> lock(jobsMutex);
        jobs.emplace(jobId, state);
    }

    std::thread([jobId, state, input, expected, opts = opts_]() mutable {
        std::vector<Variable> vars;
        if (!input.empty()) {
            const auto cols = input[0].size();
            for (size_t i = 0; i < cols; ++i) vars.emplace_back("x" + std::to_string(i));
        }
        auto cancelFlag = std::make_shared<std::atomic<bool>>(false);
        state->cancelFlag = cancelFlag;
        Solver solver(std::move(vars), input, expected, opts.useEnhanced, opts.useUltra, opts.target, opts.threads, opts.timeLimit);
        solver.setCancelFlag(cancelFlag);
        solver.setProgressCallback([jobId](number p) {
            std::lock_guard<std::mutex> lock(jobsMutex);
            auto it = jobs.find(jobId);
            if (it != jobs.end()) {
                // store progress by invoking global callback if set
                if (globalProgressCb) globalProgressCb(p);
            }
        });
        solver.start();
        if (cancelFlag->load() || state->cancelled.load()) {
            state->done.store(true);
            return;
        }
        Result r;
        const auto hall = solver.hallOfFameCopy();
        if (!hall.empty()) {
            const auto best = *hall.rbegin();
            r.formula = best.getFormula().toString();
            r.score = static_cast<double>(best.getRate());
        }
        r.metrics.duration_seconds = solver.elapsed_seconds();
        r.metrics.threads = solver.threads();
        state->result = r;
        state->done.store(true);
    }).detach();

    return jobId;
}

std::optional<Result> Context::pollResult(const JobId& id) {
    std::lock_guard<std::mutex> lock(jobsMutex);
    auto it = jobs.find(id);
    if (it == jobs.end()) return {};
    auto state = it->second;
    if (!state->done.load()) return {};
    return state->result;
}

void Context::cancel(const JobId& id) {
    std::lock_guard<std::mutex> lock(jobsMutex);
    auto it = jobs.find(id);
    if (it == jobs.end()) return;
    it->second->cancelled.store(true);
    if (it->second->cancelFlag) it->second->cancelFlag->store(true);
}

void Context::setProgressCallback(ProgressCallback cb) {
    globalProgressCb = cb;
}

std::string Result::to_json() const {
    std::ostringstream oss;
    oss << "{ \"formula\": \"" << formula << "\", \"score\": " << score << ", \"duration_seconds\": " << metrics.duration_seconds << " }";
    return oss.str();
}
}