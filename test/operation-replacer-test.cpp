#include "test.hpp"
#include "../src/solver/creators/operation-replacer.hpp"

Variable x("x", 1);
auto var_x = std::make_shared<Variable>(x);
OperationReplacer operationReplacer(testCoin, testRandomNumber);

TEST_CASE("Unary operations") {
    Formula formula(std::make_shared<Sine>(var_x), x);
    CHECK(formula.toString() == "sin(x)");

    formula = operationReplacer.change(formula);
    CHECK(formula.toString() == "sin(x)");
}

TEST_CASE("Binary operations") {
    Number n(4);
    Formula formula(std::make_shared<Addition>(var_x, std::make_shared<Number>(4)), x);
    CHECK(formula.toString() == "(x+4)");

    formula = operationReplacer.change(formula);
    CHECK(formula.toString() == "(x+4)");
}

TEST_CASE("Mixed operations") {
    Number n(4);
    Formula formula(
        std::make_shared<Square>(
            std::make_shared<Division>(
                std::make_shared<Number>(7),
                std::make_shared<Addition>(var_x, std::make_shared<Number>(4))
            )
        ),
        x
    );
    CHECK(formula.toString() == "((7/(x+4)))^2");

    formula = operationReplacer.change(formula);
    CHECK(formula.toString() == "((7/(x+4)))^2");
}