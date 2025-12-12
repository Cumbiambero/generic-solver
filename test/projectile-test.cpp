#include "test.hpp"
#include "../src/utils/csv.hpp"
#include <cmath>

TEST_CASE("Projectile Range") {
    const auto &input = parseCSV("resources/projectile-input.csv");
    const auto &results = parseCSV("resources/projectile-results.csv");
    REQUIRE(input.size() == results.size());

    const long double g = 9.80665L;
    for (std::size_t i = 0; i < input.size(); ++i) {
        const long double v = input[i][0];
        const long double theta_deg = input[i][1];
        const long double theta = theta_deg * M_PI / 180.0L;
        const long double expected = (v * v) * std::sin(2.0L * theta) / g;
        CHECK(std::abs(results[i][0] - expected) / (std::abs(expected) < 1e-12L ? 1.0L : std::abs(expected)) < 1e-3L);
    }
}
