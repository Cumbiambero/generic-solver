#include "test.hpp"

TEST_CASE("Additions") {
    auto n22 = std::make_shared<Number>(22);
    auto n8 = std::make_shared<Number>(8);
    Addition additionPositive(n22, n8);
    CHECK((additionPositive.calculate() == 30));

    auto n_25 = std::make_shared<Number>(-25);
    auto n_9 = std::make_shared<Number>(-9);
    Addition additionNegative(n_25, n_9);
    CHECK((additionNegative.calculate() == -34));

    Addition additionPositiveNegative(
        std::make_shared<Addition>(n22, n8),
        std::make_shared<Addition>(n_25, n_9)
    );
    CHECK(additionPositiveNegative.toString() == "((22+8)+((-25)+(-9)))");
    CHECK((additionPositiveNegative.calculate() == -4));

    Addition additionTwicePositive(
        std::make_shared<Addition>(n22, n8),
        std::make_shared<Addition>(n22, n8)
    );
    CHECK(additionTwicePositive.toString() == "((22+8)+(22+8))");
    CHECK((additionTwicePositive.calculate() == 60));

    auto n_05 = std::make_shared<Number>(0.499999);
    auto n_000001 = std::make_shared<Number>(0.000001);
    CHECK(format(Addition(n_05, n_000001).calculate()) == "0.5");
}

TEST_CASE("Subtractions") {
    auto n22 = std::make_shared<Number>(22);
    auto n8 = std::make_shared<Number>(8);
    auto n28 = std::make_shared<Number>(28);
    auto n_5 = std::make_shared<Number>(-5);
    CHECK((Subtraction(n22, n8).calculate() == 14));
    CHECK((Subtraction(n28, n_5).calculate() == 33));
    auto pi1 = std::make_shared<Pi>();
    auto pi2 = std::make_shared<Pi>();
    auto e1 = std::make_shared<Euler>();
    auto e2 = std::make_shared<Euler>();
    CHECK((Subtraction(pi1, pi2).calculate() == 0));
    CHECK((Subtraction(e1, e2).calculate() == 0));
}

TEST_CASE("Multiplications") {
    auto n31 = std::make_shared<Number>(31);
    auto n3 = std::make_shared<Number>(3);
    CHECK((Multiplication(n31, n3).calculate() == 93));

    int intMax = 2147483647;
    auto nMax = std::make_shared<Number>(intMax);
    CHECK((Multiplication(nMax, nMax).calculate() == 4611686014132420609));
}

TEST_CASE("Divisions") {
    auto n24 = std::make_shared<Number>(24);
    auto n9 = std::make_shared<Number>(9);
    auto n1 = std::make_shared<Number>(1);
    auto n0 = std::make_shared<Number>(0);
    auto n3_2 = std::make_shared<Number>(3);
    CHECK(format(Division(n24, n9).calculate()) == "2.6666667");
    CHECK((Division(n1, n0).calculate() == std::numeric_limits<number>::infinity()));

    auto div13 = std::make_shared<Division>(n1, n3_2);
    CHECK(format(
        Addition(
            std::make_shared<Addition>(div13, div13),
            div13
        ).calculate()) == "1");
}

TEST_CASE("Square") {
    auto n_5_2 = std::make_shared<Number>(-5);
    CHECK((Square(n_5_2).calculate() == 25));
    auto sq = std::make_shared<Square>(n_5_2);
    auto wrap = std::make_shared<Wrapper>(sq);
    CHECK((Square(wrap).calculate() == 625));
}

TEST_CASE("SquareRoot") {
    auto n25 = std::make_shared<Number>(25);
    CHECK((SquareRoot(n25).calculate() == 5));
    CHECK((SquareRootNegative(n25).calculate() == -5));
}

TEST_CASE("Trigonometric") {
    auto n1_2 = std::make_shared<Number>(1);
    CHECK(format(Sine(n1_2).calculate()) == "0.84147098");

    auto pi3 = std::make_shared<Pi>();
    auto pi4 = std::make_shared<Pi>();
    Addition twoPi(pi3, pi4);
    auto twoPiPtr = std::make_shared<Addition>(pi3, pi4);
    CHECK(format(Addition(std::make_shared<Cosine>(twoPiPtr), std::make_shared<Sine>(twoPiPtr)).calculate()) == "1");
    CHECK(format(Cosine(std::make_shared<Cosine>(twoPiPtr)).calculate()) == "1");
}