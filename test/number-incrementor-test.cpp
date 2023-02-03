#include "test.hpp"
#include "../src/solver/changers/incrementor-by-one.hpp"
#include "../src/solver/changers/incrementor-by-fragement.hpp"
#include "../src/solver/changers/decrementor-by-one.hpp"
#include "../src/solver/changers/decrementor-by-fragment.hpp"

Variable x("x", 1);
DecrementorByOne decrementorByOne(testCoin);
DecrementorByFragment decrementorByFragment(testCoin);
IncrementorByOne incrementorByOne(testCoin);
IncrementorByFragment incrementorByFragment(testCoin);

TEST_CASE("Increment") {
    Division division(x, Number(2));
    Formula formula(division, x);
    CHECK(formula.toString() == "(x/2)");

    incrementorByOne.change(formula);
    CHECK(formula.toString() == "(x/3)");

    for(int i = 0; i < 100; ++i) {
        incrementorByOne.change(formula);
    }
    CHECK(formula.toString() == "(x/53)");

    for(int i = 0; i < 10; ++i) {
        decrementorByOne.change(formula);
    }
    CHECK(formula.toString() == "(x/48)");

    for(int i = 0; i < 1000; ++i) {
        incrementorByFragment.change(formula);
    }
    CHECK(formula.toString() == "(x/48)");

    for(int i = 0; i < 100000000; ++i) {
        decrementorByFragment.change(formula);
    }
    CHECK(formula.toString() == "(x/48)");

}