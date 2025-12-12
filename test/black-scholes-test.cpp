#include "test.hpp"
#include "../src/utils/csv.hpp"
#include <cmath>

// Black-Scholes cumulative normal
static long double N(long double x) {
    return 0.5L * (1.0L + std::erf(x / std::sqrt(2.0L)));
}

static long double bs_call(long double S, long double K, long double r, long double T, long double sigma) {
    if (T <= 0.0L || sigma <= 0.0L) return std::max(0.0L, S - K);
    long double d1 = (std::log(S / K) + (r + 0.5L * sigma * sigma) * T) / (sigma * std::sqrt(T));
    long double d2 = d1 - sigma * std::sqrt(T);
    return S * N(d1) - K * std::expl(-r * T) * N(d2);
}

TEST_CASE("Black-Scholes Call Prices") {
    const auto &input = parseCSV("resources/black-scholes-input.csv");
    const auto &results = parseCSV("resources/black-scholes-results.csv");
    REQUIRE(input.size() == results.size());

    for (std::size_t i = 0; i < input.size(); ++i) {
        const long double S = input[i][0];
        const long double K = input[i][1];
        const long double r = input[i][2];
        const long double T = input[i][3];
        const long double sigma = input[i][4];
        const long double expected = results[i][0];
        const long double computed = bs_call(S, K, r, T, sigma);
        CHECK(std::abs(computed - expected) / (expected == 0 ? 1.0L : expected) < 1e-6L);
    }
}
