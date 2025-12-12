#include "test.hpp"
#include "../src/solver/creators/operation-replacer.hpp"

OperationReplacer operationReplacer(testCoin, testRandomNumber);

TEST_CASE("Unary operations") {
    Variable x("x", 5);
    auto var_x = std::make_shared<Variable>(x);
    
    Formula formula(std::make_shared<Sine>(var_x), x);
    CHECK(formula.toString().find("sin") != string::npos);
    CHECK(formula.toString().find("x") != string::npos);

    formula = operationReplacer.change(formula);
    CHECK(formula.toString().find("x") != string::npos);
}

TEST_CASE("Binary operations") {
    Variable x("x", 5);
    auto var_x = std::make_shared<Variable>(x);
    
    Formula formula(std::make_shared<Addition>(var_x, std::make_shared<Number>(4)), x);
    CHECK(formula.toString().find("x") != string::npos);
    CHECK(formula.toString().find("4") != string::npos);

    formula = operationReplacer.change(formula);
    CHECK(formula.toString().find("x") != string::npos);
}

TEST_CASE("Mixed operations") {
    Variable x("x", 5);
    auto var_x = std::make_shared<Variable>(x);
    
    Formula formula(
        std::make_shared<Square>(
            std::make_shared<Division>(
                std::make_shared<Number>(7),
                std::make_shared<Addition>(var_x, std::make_shared<Number>(4))
            )
        ),
        x
    );
    string original = formula.toString();
    CHECK(original.find("7") != string::npos);
    CHECK(original.find("x") != string::npos);
    CHECK(original.find("4") != string::npos);

    formula = operationReplacer.change(formula);
    string changed = formula.toString();
    CHECK(changed.find("x") != string::npos);
}