#ifndef GENERIC_SOLVER_SOLVER_HPP
#define GENERIC_SOLVER_SOLVER_HPP

#include "../tree/tree.hpp"
#include <cmath>
#include <set>
#include <vector>

class Solution {
public:
    Solution(Node &formula, double rate) : formula(formula), rate(rate) {}

    bool operator<(Solution &other) const { return (this->rate < other.rate); }

    Node &getForumla() { return this->getForumla(); }

    int getRate() { return this->getRate(); }

private:
    Node &formula;
    int rate;
};

class Result {
public:
    static number rate(const number actual, const number expected) {
        return 1 - abs(1 - expected / actual);
    }

    static number rate(const Node &formula, const vector<vector<number>> &input,
                       const vector<vector<number>> &expected) {
        number result = 0;
        size_t records = expected.size();

        for (int i = 0; i < records; ++i) {
            for (int j = 0; j < records; ++j) {
                result += (expected[i][j] / input[i][j]) / records;
            }
        }
        return result == 0 ? 0 : 1 - abs(1 - result);
    }
};

set<Solution> solutions;
#endif