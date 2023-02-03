#ifndef GENERIC_SOLVER_SOLVER_HPP
#define GENERIC_SOLVER_SOLVER_HPP

#include "solver-base.hpp"
#include "creators/operation-producer.hpp"
#include <cmath>
#include <thread>
#include <utility>

class Solution {
public:
    Solution(Formula &formula, ChangerType changer, number rate) : formula(formula), lastChanger(changer), rate(rate) {}

    bool operator<(Solution &other) const { return (this->rate < other.rate); }

    [[nodiscard]] Formula &getForumla() const { return formula; }

    [[nodiscard]] ChangerType getLastChanger() const { return lastChanger; }

    [[nodiscard]] number getRate() const { return rate; }

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
            result += expected[i][0] / formula.evaluate(input[i][0]) / records;
        }
        return result == 0 ? 0 : 1 - abs(1 - result);
    }
};

class Solver {
public:
    template<typename R>
    explicit Solver(vector<Variable> variables, R &randomNumber) : variables(std::move(variables)),
                                                                   cores(thread::hardware_concurrency()),
                                                                   currentState(READY), operationProducer(
                    make_shared<OperationProducer>(OperationProducer(randomNumber))) {}

    Solver(vector<Variable> variables) : variables(std::move(variables)), cores(thread::hardware_concurrency()),
                                         currentState(READY), operationProducer(make_shared<OperationProducer>()) {}

    void start() {
        const shared_ptr<Node> &operation = operationProducer->produce(variables);
        Formula formula(operation, variables);

        // TODO: Alter formula with changers and rate the results. Store them in a list and prefer the best changers.

    }

    [[nodiscard]] SolverState getState() const {
        return currentState;
    }

    void updateState(SolverState state) {
        this->currentState = state;
    }

private:
    shared_ptr<OperationProducer> operationProducer;
    SolverState currentState;
    vector<Variable> variables;
    unsigned int cores;
};

set<Solution> solutions;
#endif