#include "test.hpp"
#include "../src/solver/creators/number-inserter.hpp"

Variable x("x", 1);
auto var_x = std::make_shared<Variable>(x);
NumberInserter numberInserter(testCoin, testRandomNumber);

TEST_CASE("Insert number") {
    Formula formulaSine(std::make_shared<Sine>(var_x), x);
    CHECK(formulaSine.toString() == "sin(x)");

    auto sineWithNumber = numberInserter.change(formulaSine);
    CHECK(sineWithNumber.toString() == "(sin(x)*2)");

    Formula formulaDivision(std::make_shared<Division>(var_x, std::make_shared<Number>(2)), x);
    CHECK(formulaDivision.toString() == "(x/2)");

    auto divisionWithNumber = numberInserter.change(formulaDivision);
    CHECK(divisionWithNumber.toString() == "((x/2)*2)");

    auto divisionWithTwoNumbers = numberInserter.change(divisionWithNumber);
    CHECK(divisionWithTwoNumbers.toString() == "(((x/2)*2)*2)");
}