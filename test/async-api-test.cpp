#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "test.hpp"
#include <thread>
#include "solver_core/solver_core.hpp"

using namespace solver_core;

TEST_CASE("runAsync returns result") {
    Options opts;
    opts.timeLimit = std::chrono::seconds{1};
    Context ctx(opts);
    InputMatrix input{{1.0L}, {2.0L}, {3.0L}};
    InputMatrix results{{2.0L}, {4.0L}, {6.0L}};

    auto job = ctx.runAsync(input, results);
    REQUIRE(!job.empty());

    std::optional<Result> res;
    for (int i = 0; i < 100 && !res; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        res = ctx.pollResult(job);
    }
    REQUIRE(res.has_value());
    CHECK(res.has_value());
}

TEST_CASE("cancel stops job") {
    Options opts;
    opts.timeLimit = std::chrono::seconds{1};
    Context ctx(opts);
    InputMatrix input(1000, std::vector<number>(4, 1.0L));
    InputMatrix results(1000, std::vector<number>(1, 1.0L));

    auto job = ctx.runAsync(input, results);
    REQUIRE(!job.empty());
    ctx.cancel(job);

    std::optional<Result> res;
    for (int i = 0; i < 100 && !res; ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        res = ctx.pollResult(job);
    }
    // after cancel, result should be empty
    CHECK(!res.has_value());
}
