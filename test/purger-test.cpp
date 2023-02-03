#include "test.hpp"
#include "../src/solver/changers/purger.hpp"

TEST_CASE("Purger") {
    Variable r("r");
    Variable i("i");
    vector<Variable> vec;
    vec.push_back(r);
    vec.push_back(i);

    Multiplication multiplication(r, i);
    Division division(multiplication, Number(2));

    Formula formula(division, vec);
    CHECK(formula.toString() == "((r*i)/2)");
    Purger purger(testCoin, testRandomNumber);
    purger.change(formula);
    CHECK(formula.toString() == "(r*i)");
}