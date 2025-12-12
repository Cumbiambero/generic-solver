#include "test.hpp"
#include "../src/utils/csv.hpp"

TEST_CASE("Hooke Law") {
    const auto &input = parseCSV("resources/hooke-input.csv");
    const auto &results = parseCSV("resources/hooke-results.csv");
    REQUIRE(input.size() == results.size());

    for (std::size_t i = 0; i < input.size(); ++i) {
        const long double k = input[i][0];
        const long double x = input[i][1];
        const long double expected = k * x;
        CHECK(std::abs(results[i][0] - expected) / (std::abs(expected) < 1e-12L ? 1.0L : std::abs(expected)) < 1e-6L);
    }
}
