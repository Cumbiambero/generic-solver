#include "test.hpp"
#include "../src/solver/changers/incrementor-by-one.hpp"
#include "../src/solver/changers/incrementor-by-fragment.hpp"
#include "../src/solver/changers/reducer-by-one.hpp"
#include "../src/solver/changers/reducer-by-fragment.hpp"
#include "../src/solver/changers/reducer-by-halving.hpp"
#include "../src/solver/changers/incrementor-by-doubling.hpp"

Variable x("x", 1);
Division division(x, Number(2));
Formula formula(division, x);
ReducerByOne reducerByOne(testCoin);
ReducerByFragment reducerByFragment(testCoin);
ReducerByHalving reducerByHalving(testCoin);
IncrementorByOne incrementorByOne(testCoin);
IncrementorByFragment incrementorByFragment(testCoin);
IncrementorByDoubling incrementorByDoubling(testCoin);

TEST_CASE("Increment and decrement") {
    CHECK(formula.toString() == "(x/2)");

    incrementorByOne.change(formula);
    CHECK(formula.toString() == "(x/3)");

    for (int i = 0; i < 100; ++i) {
        incrementorByOne.change(formula);
    }
    CHECK(formula.toString() == "(x/53)");

    for (int i = 0; i < 10; ++i) {
        reducerByOne.change(formula);
    }
    CHECK(formula.toString() == "(x/48)");
}

TEST_CASE("Smallest fractions") {
    number beforeIncrement(formula.getNumbers()[0]->calculate());
    for(int i = 0; i < 10; ++i) {
        incrementorByFragment.change(formula);
    }
    number afterIncrement(formula.getNumbers()[0]->calculate());

    CHECK((beforeIncrement < afterIncrement));

    number beforeDecrement(formula.getNumbers()[0]->calculate());
    for(int i = 0; i < 10; ++i) {
        reducerByFragment.change(formula);
    }
    number afterDecrement(formula.getNumbers()[0]->calculate());

    CHECK((beforeDecrement > afterDecrement));
    CHECK((beforeIncrement == afterDecrement));
}

TEST_CASE("Halving and doubling") {
    formula.getNumbers()[0]->setValue(60);
    CHECK(formula.toString() == "(x/60)");

    incrementorByDoubling.change(formula);
    CHECK(formula.toString() == "(x/120)");

    reducerByHalving.change(formula);
    CHECK(formula.toString() == "(x/60)");

    for(int i = 0; i < 10; ++i) {
        reducerByHalving.change(formula);
    }
    CHECK(formula.toString() == "(x/1.875)");
}