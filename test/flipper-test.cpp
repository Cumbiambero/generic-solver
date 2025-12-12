#include "test.hpp"
#include "../src/solver/changers/flipper.hpp"

TEST_CASE("Flipper") {
    Variable x("x", 2);
    auto var_x = std::make_shared<Variable>(x);
    auto euler = std::make_shared<Euler>();
    auto cube_root = std::make_shared<CubeRoot>(var_x);
    auto mult = std::make_shared<Multiplication>(cube_root, euler);
    auto tangent = std::make_shared<Tangent>(3);
    auto addition = std::make_shared<Addition>(mult, tangent);
    auto division = std::make_shared<Division>(std::make_shared<Number>(4), var_x);
    auto power = std::make_shared<Power>(addition, division);
    Formula formula(power, x);
    
    string original = formula.toString();
    CHECK(original.find("cbrt") != string::npos);
    CHECK(original.find("e") != string::npos);
    CHECK(original.find("tan") != string::npos);

    Flipper flipper(testCoin);
    formula = flipper.change(formula);
    string flipped = formula.toString();
    CHECK(flipped != original);
}

TEST_CASE("Flipper immutability") {
    Variable a("a", 1);
    auto var_a = std::make_shared<Variable>(a);
    auto sqrt_a = std::make_shared<SquareRoot>(var_a);
    auto mult_a = std::make_shared<Multiplication>(sqrt_a, 8);
    auto addition_a = std::make_shared<Addition>(mult_a, 3);
    Formula formula(addition_a, a);
    string original = formula.toString();
    CHECK(original.find("sqrt") != string::npos);

    Flipper flipper(testCoin);
    auto sqrt_a2 = std::make_shared<SquareRoot>(var_a);
    auto mult_a2 = std::make_shared<Multiplication>(sqrt_a2, 8);
    auto addition_a2 = std::make_shared<Addition>(mult_a2, 3);
    Formula copy(addition_a2, a);
    copy = flipper.change(copy);

    CHECK(formula.toString() == original);
    CHECK(copy.toString() != formula.toString());
}