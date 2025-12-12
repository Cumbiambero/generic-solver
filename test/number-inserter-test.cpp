#include "test.hpp"
#include "../src/solver/creators/number-inserter.hpp"

NumberInserter numberInserter(testCoin, testRandomNumber);

TEST_CASE("Insert number") {
    Variable x("x", 5);
    auto var_x = std::make_shared<Variable>(x);
    
    Formula formulaSine(std::make_shared<Sine>(var_x), x);
    CHECK(formulaSine.toString().find("sin") != string::npos);

    auto sineWithNumber = numberInserter.change(formulaSine);
    string sineResult = sineWithNumber.toString();
    CHECK(sineResult.find("sin") != string::npos);
    CHECK(sineResult.length() > formulaSine.toString().length());

    Formula formulaDivision(std::make_shared<Division>(var_x, std::make_shared<Number>(2)), x);
    CHECK(formulaDivision.toString().find("x") != string::npos);

    auto divisionWithNumber = numberInserter.change(formulaDivision);
    string divResult = divisionWithNumber.toString();
    CHECK(divResult.length() > 0);

    auto divisionWithTwoNumbers = numberInserter.change(divisionWithNumber);
    CHECK(divisionWithTwoNumbers.toString().length() >= divResult.length());
}