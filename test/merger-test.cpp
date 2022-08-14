#include "test.hpp"
#include "../src/creators/merger.hpp"

TEST_CASE("Merger") {
    Variable x("x");
    vector<Variable> vec;
    vec.push_back(x);

    Multiplication circleArea(Square(x), PI);
    Multiplication volume(x, Addition(x, Number(7)));

    Formula formulaOne(circleArea, vec);
    CHECK(formulaOne.toString() == "((x)²*π)");
    Formula formulaTwo(volume, vec);
    CHECK(formulaTwo.toString() == "(x*(x+7))");
    Merger merger(testCoin, testRandomNumber);
    Formula formulaThree = merger.merge(formulaOne, formulaTwo);
    CHECK(formulaThree.toString() == "((x)²*(x+7))");
}