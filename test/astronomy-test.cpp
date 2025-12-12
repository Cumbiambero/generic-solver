#include "test.hpp"
#include "../src/utils/csv.hpp"
#include <cmath>

TEST_CASE("Kepler Third Law - Orbital Period") {
    const auto &input = parseCSV("resources/astronomy-input.csv");
    const auto &results = parseCSV("resources/astronomy-results.csv");
    REQUIRE(input.size() == results.size());

    // Earth's gravitational parameter mu = 398600.4418 km^3 / s^2
    const long double mu = 398600.4418L;

    for (std::size_t i = 0; i < input.size(); ++i) {
        const long double a = input[i][0]; // km
        const long double expected_hours = results[i][0];
        const long double period_seconds = 2.0L * M_PI * std::sqrt((a*a*a) / mu);
        const long double period_hours = period_seconds / 3600.0L;
        CHECK(std::abs(period_hours - expected_hours) / expected_hours < 1e-6L);
    }
}
