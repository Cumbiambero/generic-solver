#include "test.hpp"
#include "../src/utils/csv.hpp"

TEST_CASE("Kinetic Energy") {
    const auto &input = parseCSV("resources/kinetic-energy-input.csv");
    const auto &results = parseCSV("resources/kinetic-energy-results.csv");
    REQUIRE(input.size() == results.size());

    for (std::size_t i = 0; i < input.size(); ++i) {
        const long double m = input[i][0];
        const long double v = input[i][1];
        const long double expected = 0.5L * m * v * v;
        CHECK(std::abs(results[i][0] - expected) < 1e-6L);
    }
}
