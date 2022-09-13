#include "test.hpp"
#include "../src/solver/creators/operation-replacer.hpp"

Variable x("x", 1);
OperationReplacer operationReplacer(testCoin, testRandomNumber);

TEST_CASE("Unary operations") {
    Sine operation(x);
    Formula formula(operation, x);
    CHECK(formula.toString() == "sin(x)");

    operationReplacer.change(formula);
    CHECK(formula.toString() == "(x)³");
}

TEST_CASE("Binary operations") {
    Number n(4);
    Addition operation(x, n);
    Formula formula(operation, x);
    CHECK(formula.toString() == "(x+4)");

    operationReplacer.change(formula);
    CHECK(formula.toString() == "(x*4)");
}

TEST_CASE("Mixed operations") {
    Number n(4);
    Addition addition(x, n);
    Division division(Number(7), addition);
    Square square(division);
    Formula formula(square, x);
    CHECK(formula.toString() == "((7/(x+4)))²");

    operationReplacer.change(formula);
    CHECK(formula.toString() == "((7/(x*4)))³");
}