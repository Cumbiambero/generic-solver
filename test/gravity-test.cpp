#include "test.hpp"
#include "../src/utils/csv.hpp"

TEST_CASE("Newton Gravity") {
    const auto &input = parseCSV("resources/gravity-input.csv");
    const auto &results = parseCSV("resources/gravity-results.csv");
    REQUIRE(input.size() == results.size());

    const long double G = 6.67408e-11L;
    for (std::size_t i = 0; i < input.size(); ++i) {
        const long double m1 = input[i][0];
        const long double m2 = input[i][1];
        const long double d = input[i][2];
        const long double expected = G * m1 * m2 / (d * d);
        CHECK(std::abs(results[i][0] - expected) / (expected == 0 ? 1.0L : expected) < 1e-6L);
    }
}
