#include "test.hpp"
#include "../src/solver/changers/flipper.hpp"

TEST_CASE("Flipper") {
    Variable x("x", 1);
    Multiplication multiplication(CubeRoot(x), E);
    Addition addition(multiplication, Tangent(Number(3)));
    Power power(addition, Division(Number(4), x));
    Formula formula(power, x);
    CHECK(formula.toString() == "(((∛(x)*e)+tan(3))^(4/x))");

    Flipper flipper(testCoin);
    flipper.change(formula);
    CHECK(formula.toString() == "((4/x)^((e*∛(x))+tan(3)))");
}

TEST_CASE("Flipper immutability") {
    Variable a("a", 1);
    Multiplication multiplication(SquareRoot(a), Number(8));
    Addition addition(multiplication, Number(3));
    Formula formula(addition, a);
    CHECK(formula.toString() == "((√(a)*8)+3)");

    Flipper flipper(testCoin);
    Formula copy(formula);
    flipper.change(copy);

    CHECK(formula.toString() == "((√(a)*8)+3)");
    CHECK(copy.toString() == "((8*√(a))+3)");
}