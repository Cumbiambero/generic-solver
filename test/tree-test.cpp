#include "test.hpp"

TEST_CASE("Additions") {
    Addition additionPositive(Number(22), Number(8));
    CHECK((additionPositive.calculate() == 30));

    Addition additionNegative(Number(-25), Number(-9));
    CHECK((additionNegative.calculate() == -34));

    Addition additionPositiveNegative(additionPositive, additionNegative);
    CHECK(additionPositiveNegative.toString() == "((22+8)+((-25)+(-9)))");
    CHECK((additionPositiveNegative.calculate() == -4));

    Addition additionTwicePositive(additionPositive, additionPositive);
    CHECK(additionTwicePositive.toString() == "((22+8)+(22+8))");
    CHECK((additionTwicePositive.calculate() == 60));

    CHECK(format(Addition(Number(0.499999), Number(0.000001)).calculate()) ==
          "0.5");
}

TEST_CASE("Subtractions") {
    CHECK((Subtraction(Number(22), Number(8)).calculate() == 14));
    CHECK((Subtraction(Number(28), Number(-5)).calculate() == 33));
    CHECK((Subtraction(PI, PI).calculate() == 0));
    CHECK((Subtraction(E, E).calculate() == 0));
}

TEST_CASE("Multiplications") {
    CHECK((Multiplication(Number(31), Number(3)).calculate() == 93));

    int intMax = 2147483647;
    CHECK((Multiplication(Number(intMax), Number(intMax)).calculate() ==
          4611686014132420609));
}

TEST_CASE("Divisions") {
    CHECK(format(Division(Number(24), Number(9)).calculate()) == "2.6666667");
    CHECK((Division(Number(1), Number(0)).calculate() ==
          numeric_limits<number>::infinity()));

    Division division(Number(1), Number(3));
    CHECK(format(Addition(Addition(division, division), division).calculate()) ==
          "1");
}

TEST_CASE("Square") {
    CHECK((Square(Number(-5)).calculate() == 25));
    CHECK((Square(Wrapper(Square(Number(-5)))).calculate() == 625));
}

TEST_CASE("SquareRoot") {
    CHECK((SquareRoot(Number(25)).calculate() == 5));
    CHECK((SquareRootNegative(Number(25)).calculate() == -5));
}

TEST_CASE("Trigonometric") {
    CHECK(format(Sine(Number(1)).calculate()) == "0.84147098");

    Addition twoPi(PI, PI);
    CHECK(format(Addition(Cosine(twoPi), Sine(twoPi)).calculate()) == "1");
    CHECK(format(Cosine(Cosine(twoPi)).calculate()) == "1");
    CHECK(format(Cosine(twoPi).calculate()) == "1");
}