#ifndef GENERIC_SOLVER_SOLVER_HPP
#define GENERIC_SOLVER_SOLVER_HPP

#include "solver-base.hpp"
#include <cmath>

class Solution {
public:
    Solution(Formula &formula, ChangerType changer, number rate) : formula(formula), lastChanger(changer), rate(rate) {}

    bool operator<(Solution &other) const { return (this->rate < other.rate); }

    Formula &getForumla() const { return formula; }

    ChangerType getLastChanger() const { return lastChanger; }

    number getRate() const { return rate; }

private:
    Formula &formula;
    ChangerType lastChanger;
    number rate;

public:

};

class Result {
public:
    static number rate(const number actual, const number expected) {
        return 1 - abs(1 - expected / actual);
    }

    static number rate(Formula &formula, const vector<vector<number>> &input,
                       const vector<vector<number>> &expected) {
        number result = 0;
        size_t records = expected.size();

        for (auto i = 0; i < records; ++i) {
            result += expected[i][0] / formula.evaluate(input[i]) / records;
        }
        return result == 0 ? 0 : 1 - abs(1 - result);
    }
};

set<Solution> solutions;
#endif