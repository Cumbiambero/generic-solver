#include "test.hpp"
#include "../src/changers/flipper.hpp"

TEST_CASE("Flipper") {
    Variable x("x", 1);
    Multiplication multiplication(CubeRoot(x), E);
    Addition addition(multiplication, Tangent(Number(3)));
    Power power(addition, Division(Number(4), x));
    Formula formula(power, x);
    CHECK(formula.toString() == "(((∛(x)*e)+tan(3))^(4/x))");

    Flipper flipper(testCoin);
    flipper.flip(formula);
    CHECK(formula.toString() == "((4/x)^((e*∛(x))+tan(3)))");
}