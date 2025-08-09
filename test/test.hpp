#ifndef GENERIC_SOLVER_SOLVER_TEST_HPP
#define GENERIC_SOLVER_SOLVER_TEST_HPP
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../src/utils/arbitrary.hpp"
#include "../src/solver/formula.hpp"
#include "doctest/doctest.h" // Ubuntu: sudo apt-get install doctest-dev or from https://github.com/doctest/doctest
#include <sstream>
#include <iomanip>

inline std::string format(number value) {
    std::ostringstream oss;
    oss << std::setprecision(8) << std::noshowpoint << value;
    return oss.str();
}

class TestCoin : public Coin {
public:
    bool toss() override {
        previous = !previous;
        return previous;
    }

private:
    bool previous;
};

class TestRandomNumber : public RandomNumber {
public:
    int calculate(int from, int to) override {
        return (from + to) / 2;
    }
};


static Pi PI;
static Euler E;

TestCoin testCoin;
TestRandomNumber testRandomNumber;

inline std::shared_ptr<RandomNumber> makeTestRandomNumber() {
    return std::make_shared<TestRandomNumber>();
}

#endif