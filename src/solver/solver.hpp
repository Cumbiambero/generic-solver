#ifndef GENERIC_SOLVER_SOLVER_HPP
#define GENERIC_SOLVER_SOLVER_HPP

#include "solver-base.hpp"
#include "creators/operation-producer.hpp"
#include "changers/flipper.hpp"
#include <cmath>
#include <thread>
#include <utility>

class Solution {
public:
    Solution(const Formula &formula, ChangerType changer, number rate) : formula(formula), lastChanger(changer),
                                                                         rate(rate) {}

    bool operator<(Solution &other) const { return (this->rate < other.rate); }

    bool operator>(const Solution &other) const {
        return other.rate < this->rate;
    }

    bool operator<=(const Solution &other) const {
        return other.rate >= this->rate;
    }

    bool operator>=(const Solution &other) const {
        return this->rate >= other.rate;
    }

    bool operator==(const Solution &other) const {
        return lastChanger == other.lastChanger &&
               rate == other.rate &&
               formula.toString() == other.formula.toString();
    }

    bool operator!=(const Solution &other) const {
        return !(other == *this);
    }

    [[nodiscard]] Formula getForumla() const { return formula; }

    [[nodiscard]] ChangerType getLastChanger() const { return lastChanger; }

    [[nodiscard]] number getRate() const { return rate; }

private:
    const Formula &formula;
    const ChangerType lastChanger;
    const number rate;
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
    explicit
    Solver(vector<Variable> variables, const vector<vector<number>> &input,
           const vector<vector<number>> &results, R &randomNumber)
            : variables(std::move(variables)), input(input), results(results),
              cores(max((unsigned int) 1, thread::hardware_concurrency())),
              currentState(READY), operationProducer(make_unique<OperationProducer>(OperationProducer(randomNumber))) {}

    Solver(vector<Variable> variables, const vector<vector<number>> &input, const vector<vector<number>> &results)
            : variables(std::move(variables)), input(input), results(results),
              cores(max((unsigned int) 1, thread::hardware_concurrency())),
              currentState(READY), operationProducer(make_unique<OperationProducer>()) {}


    void start() {
        if (currentState == READY) {
            Formula formula(operationProducer->produce(variables), variables);
            Solution solution(formula, FLIPPER, 0.0);
            this->solutions.insert(make_unique<Solution>(solution));
        }
        currentState = RUNNING;

        for (int i = 0; i < cores; ++i) {
            thread worker([this]() {
                while (currentState == RUNNING) {
                    work();
                }
            });
            worker.join();
        }
    }

    void pause() {
        currentState = PAUSED;
    }

    [[nodiscard]] SolverState getState() const {
        return currentState;
    }

    void updateState(SolverState state) {
        this->currentState = state;
    }

private:
    const vector<Variable> variables;
    const vector<vector<number>> input;
    const vector<vector<number>> results;
    ChangerPicker changerPicker;
    unique_ptr<OperationProducer> operationProducer;
    SolverState currentState;

    set<unique_ptr<Solution>> solutions;
    unsigned int cores;
    atomic<unsigned int> pos = 0;

    void work() {
        auto changer = pickChanger();
        auto formula = changer->change(solutions.begin()->get()->getForumla()); // TODO: Fix SISSEGV that happens here
        number rate = Result::rate(formula, input, results);
        Solution solution(formula, changer->getType(), rate);
        solutions.insert(make_unique<Solution>(solution));
        if (rate > 0.9999999999) { // TODO: make this value configurable
            currentState = DONE;
        }
    }

    Changer *pickChanger() {
        if (pos++ == cores) {
            pos = 0;
            return changerPicker.pickRandomChanger();
        }
        auto it = solutions.begin();

        // advance(it, atomic_load(&pos)); // TODO: try to use more than only the first position

        return changerPicker.pickChanger(it->get()->getLastChanger());
    }

};

#endif