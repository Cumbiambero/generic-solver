#include "test.hpp"
#include "../src/utils/csv.hpp"

TEST_CASE("CSV Header Tolerance") {
    const auto &data = parseCSV("resources/csv-with-header.csv");
    REQUIRE(data.size() == 3);
    CHECK(data[0][0] == 1);
    CHECK(data[0][1] == 2);
    CHECK(data[2][0] == 5);
    CHECK(data[2][1] == 6);
}
