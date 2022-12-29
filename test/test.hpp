#ifndef GENERIC_SOLVER_SOLVER_TEST_HPP
#define GENERIC_SOLVER_SOLVER_TEST_HPP
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "../src/utils/arbitrary.hpp"
#include "../src/solver/formula.hpp"
#include "doctest/doctest.h" // Ubuntu: sudo apt-get install doctest-dev or from https://github.com/doctest/doctest

string format(number value) {
    ostringstream oss;
    oss << setprecision(8) << noshowpoint << value;
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

TestCoin testCoin;
TestRandomNumber testRandomNumber;

#endif