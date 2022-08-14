#include "test.hpp"

TEST_CASE("Formula evaluation") {
    Variable r("r", 1);
    Multiplication circleArea(Square(r), PI);
    Formula formula(circleArea, r);
    CHECK(formula.toString() == "((r)²*π)");
    CHECK((formula.evaluate(1) == PI.calculate()));
    CHECK(format(formula.evaluate(4)) == "50.265482");
    CHECK(format(formula.evaluate(200)) == "125663.71");
}