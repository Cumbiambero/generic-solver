#include "test.hpp"
#include "../src/solver/changers/incrementor-by-one.hpp"
#include "../src/solver/changers/incrementor-by-fragment.hpp"
#include "../src/solver/changers/reducer-by-one.hpp"
#include "../src/solver/changers/reducer-by-fragment.hpp"
#include "../src/solver/changers/reducer-by-halving.hpp"
#include "../src/solver/changers/incrementor-by-doubling.hpp"

Variable x("x", 1);
auto var_x = std::make_shared<Variable>(x);
Formula formula(std::make_shared<Division>(var_x, std::make_shared<Number>(2)), x);
ReducerByOne reducerByOne(testCoin);
ReducerByFragment reducerByFragment(testCoin);
ReducerByHalving reducerByHalving(testCoin);
IncrementorByOne incrementorByOne(testCoin);
IncrementorByFragment incrementorByFragment(testCoin);
IncrementorByDoubling incrementorByDoubling(testCoin);

TEST_CASE("Increment and decrement") {
    CHECK(formula.toString() == "(x/2)");

    formula = incrementorByOne.change(formula);
    CHECK(formula.toString() == "(x/3)");

    for (int i = 0; i < 100; ++i) {
        formula = incrementorByOne.change(formula);
    }
    CHECK(formula.toString() == "(x/53)");

    for (int i = 0; i < 10; ++i) {
        formula = reducerByOne.change(formula);
    }
    CHECK(formula.toString() == "(x/48)");
}

TEST_CASE("Smallest fractions") {
    number beforeIncrement(formula.getNumbers()[0]->calculate());
    for(int i = 0; i < 10; ++i) {
        formula = incrementorByFragment.change(formula);
    }
    number afterIncrement(formula.getNumbers()[0]->calculate());

    CHECK((beforeIncrement < afterIncrement));

    number beforeDecrement(formula.getNumbers()[0]->calculate());
    for(int i = 0; i < 10; ++i) {
        formula = reducerByFragment.change(formula);
    }
    number afterDecrement(formula.getNumbers()[0]->calculate());

    CHECK((beforeDecrement > afterDecrement));
    CHECK((beforeIncrement == afterDecrement));
}

TEST_CASE("Halving and doubling") {
    formula.getNumbers()[0]->setValue(60);
    CHECK(formula.toString() == "(x/60)");

    formula = incrementorByDoubling.change(formula);
    CHECK(formula.toString() == "(x/120)");

    formula = reducerByHalving.change(formula);
    CHECK(formula.toString() == "(x/60)");

    for(int i = 0; i < 10; ++i) {
        formula = reducerByHalving.change(formula);
    }
    CHECK(formula.toString() == "(x/1.875)");
}