#include "test.hpp"
#include "../src/solver/creators/number-inserter.hpp"

Variable x("x", 1);
NumberInserter operationReplacer(testCoin, testRandomNumber);

TEST_CASE("Insert number") {
    Sine sine(x);
    Formula formulaSine(sine, x);
    CHECK(formulaSine.toString() == "sin(x)");

    auto sineWithNumber = operationReplacer.insert(formulaSine);
    CHECK(sineWithNumber.toString() == "(sin(x)*1)");

    Division division(x, Number(2));
    Formula formulaDivision(division, x);
    CHECK(formulaDivision.toString() == "(x/2)");

    auto divisionWithNumber = operationReplacer.insert(formulaDivision);
    CHECK(divisionWithNumber.toString() == "((x/2)*1)");

    auto divisionWithTwoNumbers = operationReplacer.insert(divisionWithNumber);
    CHECK(divisionWithTwoNumbers.toString() == "(((x/2)*1)*1)");
}