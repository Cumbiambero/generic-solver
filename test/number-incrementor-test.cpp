#include "test.hpp"
#include "../src/solver/changers/incrementor-by-one.hpp"
#include "../src/solver/changers/incrementor-by-fragment.hpp"
#include "../src/solver/changers/reducer-by-one.hpp"
#include "../src/solver/changers/reducer-by-fragment.hpp"
#include "../src/solver/changers/reducer-by-halving.hpp"
#include "../src/solver/changers/incrementor-by-doubling.hpp"

ReducerByOne reducerByOne(testCoin);
ReducerByFragment reducerByFragment(testCoin);
ReducerByHalving reducerByHalving(testCoin);
IncrementorByOne incrementorByOne(testCoin);
IncrementorByFragment incrementorByFragment(testCoin);
IncrementorByDoubling incrementorByDoubling(testCoin);

TEST_CASE("Increment and decrement") {
    Variable x("x", 5);
    auto var_x = std::make_shared<Variable>(x);
    Formula formula(std::make_shared<Division>(var_x, std::make_shared<Number>(2)), x);
    
    number initialValue = formula.getNumbers()[0]->calculate();
    CHECK(initialValue == doctest::Approx(2.0L));

    formula = incrementorByOne.change(formula);
    number afterIncrement = formula.getNumbers()[0]->calculate();
    CHECK(afterIncrement == doctest::Approx(3.0L));

    for (int i = 0; i < 100; ++i) {
        formula = incrementorByOne.change(formula);
    }
    CHECK(formula.getNumbers()[0]->calculate() == doctest::Approx(53.0L));

    for (int i = 0; i < 10; ++i) {
        formula = reducerByOne.change(formula);
    }
    CHECK(formula.getNumbers()[0]->calculate() == doctest::Approx(48.0L));
}

TEST_CASE("Smallest fractions") {
    Variable x("x", 5);
    auto var_x = std::make_shared<Variable>(x);
    Formula formula(std::make_shared<Division>(var_x, std::make_shared<Number>(48)), x);
    
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
    CHECK((beforeIncrement == doctest::Approx(afterDecrement)));
}

TEST_CASE("Halving and doubling") {
    Variable x("x", 5);
    auto var_x = std::make_shared<Variable>(x);
    Formula formula(std::make_shared<Division>(var_x, std::make_shared<Number>(60)), x);
    
    CHECK(formula.getNumbers()[0]->calculate() == doctest::Approx(60.0L));

    formula = incrementorByDoubling.change(formula);
    CHECK(formula.getNumbers()[0]->calculate() == doctest::Approx(120.0L));

    formula = reducerByHalving.change(formula);
    CHECK(formula.getNumbers()[0]->calculate() == doctest::Approx(60.0L));

    for(int i = 0; i < 10; ++i) {
        formula = reducerByHalving.change(formula);
    }
    CHECK(formula.getNumbers()[0]->calculate() == doctest::Approx(1.875L));
}