#include "test.hpp"
#include "../src/solver/changers/flipper.hpp"

TEST_CASE("Flipper") {
    Variable x("x", 1);
    auto var_x = std::make_shared<Variable>(x);
    auto euler = std::make_shared<Euler>();
    auto cube_root = std::make_shared<CubeRoot>(var_x);
    auto mult = std::make_shared<Multiplication>(cube_root, euler);
    auto tangent = std::make_shared<Tangent>(3);
    auto addition = std::make_shared<Addition>(mult, tangent);
    auto division = std::make_shared<Division>(4, var_x);
    auto power = std::make_shared<Power>(addition, division);
    Formula formula(power, x);
    CHECK(formula.toString() == "(((cbrt(x)*e)+tan(3))^(4/x))");

    Flipper flipper(testCoin);
    formula = flipper.change(formula);
    CHECK(formula.toString() == "(((e*cbrt(x))+tan(3))^(x/4))");
}

TEST_CASE("Flipper immutability") {
    Variable a("a", 1);
    auto var_a = std::make_shared<Variable>(a);
    auto sqrt_a = std::make_shared<SquareRoot>(var_a);
    auto mult_a = std::make_shared<Multiplication>(sqrt_a, 8);
    auto addition_a = std::make_shared<Addition>(mult_a, 3);
    Formula formula(addition_a, a);
    CHECK(formula.toString() == "((sqrt(a)*8)+3)");

    Flipper flipper(testCoin);
    auto sqrt_a2 = std::make_shared<SquareRoot>(var_a);
    auto mult_a2 = std::make_shared<Multiplication>(sqrt_a2, 8);
    auto addition_a2 = std::make_shared<Addition>(mult_a2, 3);
    Formula copy(addition_a2, a);
    copy = flipper.change(copy);

    CHECK(formula.toString() == "((sqrt(a)*8)+3)");
    CHECK(copy.toString() == "((8*sqrt(a))+3)");
}