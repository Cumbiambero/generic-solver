#include "test.hpp"
#include "../src/solver/changers/purger.hpp"

TEST_CASE("Purger") {
    Variable r("r");
    Variable i("i");
    vector<Variable> vec;
    vec.push_back(r);
    vec.push_back(i);

    auto var_r = std::make_shared<Variable>(r);
    auto var_i = std::make_shared<Variable>(i);
    Formula formula(std::make_shared<Division>(
        std::make_shared<Multiplication>(var_r, var_i),
        std::make_shared<Number>(2)
    ), vec);
    CHECK(formula.toString() == "((r*i)/2)");
    Purger purger(testCoin, testRandomNumber);
    formula = purger.change(formula);
    CHECK(formula.toString() == "(r*i)");
}