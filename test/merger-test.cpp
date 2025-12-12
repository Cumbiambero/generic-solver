#include "test.hpp"
#include "../src/solver/creators/merger.hpp"

TEST_CASE("Merger") {
    Variable x("x", 2);
    vector<Variable> vec;
    vec.push_back(x);

    auto var_x = std::make_shared<Variable>(x);
    Formula formulaOne(std::make_shared<Multiplication>(
        std::make_shared<Square>(var_x),
        std::make_shared<Pi>()
    ), vec);
    CHECK(formulaOne.toString().find("x") != string::npos);
    CHECK(formulaOne.toString().find("π") != string::npos);
    
    Formula formulaTwo(std::make_shared<Multiplication>(
        var_x,
        std::make_shared<Addition>(var_x, std::make_shared<Number>(7))
    ), vec);
    CHECK(formulaTwo.toString().find("x") != string::npos);
    CHECK(formulaTwo.toString().find("7") != string::npos);
    
    Merger merger(testCoin, testRandomNumber);
    Formula formulaThree = merger.merge(formulaOne, formulaTwo);
    CHECK(formulaThree.toString().find("x") != string::npos);
}