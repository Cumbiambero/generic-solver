#include "test.hpp"
#include "../src/solver/creators/operation-producer.hpp"

TEST_CASE("Formula evaluation square") {
    Variable d("d");
    Square square(d);
    Formula formula(square, d);
    CHECK(formula.toString() == "(d)²");
    CHECK(format(formula.evaluate(5)) == "25");
}

TEST_CASE("Formula evaluation multiplication") {
    Variable r("r", 1);
    Multiplication circleArea(Square(r), PI);
    Formula formula(circleArea, r);
    CHECK(formula.toString() == "((r)²*π)");
    CHECK((formula.evaluate(1) == PI.calculate()));
    CHECK(format(formula.evaluate(4)) == "50.265482");
    CHECK(format(formula.evaluate(200)) == "125663.71");
}

TEST_CASE("Formula evaluation operation producer") {
    OperationProducer operationProducer(testRandomNumber);
    vector<Variable> vec;
    vec.emplace_back("x");
    auto node = operationProducer.produce(vec);
    Formula formula(node, vec);
    CHECK(formula.toString() == "(x)³");
    CHECK(format(formula.evaluate(2)) == "8");
}