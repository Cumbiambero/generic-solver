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
    Solution(Formula formula, ChangerType lastChanger, number rate)
            : formula(formula), lastChanger(lastChanger), rate(rate) {}

    Solution(const Solution &copy)
            : formula(copy.formula), lastChanger(copy.lastChanger), rate(copy.rate) {}

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

    [[nodiscard]] Formula getFormula() { return formula; }

    [[nodiscard]] ChangerType getLastChanger() { return lastChanger; }

    [[nodiscard]] number getRate() { return rate; }

private:
    Formula formula;
    ChangerType lastChanger;
    number rate;
};

class Evaluator {
public:
    static number rate(Formula &formula, const vector<vector<number>> &input, const vector<vector<number>> &expected) {
        number result = 0;
        number records = 0;
        for (int l = 0; l < expected.size(); l++) {
            for (int c = 0; c < expected[l].size(); c++) {
                number currentResult = formula.evaluate(input[l][c]);
                number expectedResult = expected[l][c];
                number dividend(min(expectedResult, currentResult));
                number divisor(max(expectedResult, currentResult));
                result += dividend / divisor;
                records++;
            }
        }
        result /= records;
        return result == 0 ? 0 : 1 - abs(1 - result);
    }
};

class Solver {
public:
    Solver(vector<Variable> variables, const vector<vector<number>> &input, const vector<vector<number>> &results)
            : variables(std::move(variables)), input(input), results(results),
              cores(max((unsigned int) 1, thread::hardware_concurrency())),
              currentState(SolverState::READY) {}

    void start() {
        if (currentState == SolverState::READY) {
            Formula formula(operationProducer.produce(variables), variables);
            this->solutions.insert(make_unique<Solution>(formula, ChangerType::OPERATION_REPLACER, 0.0).get());
        }
        currentState = SolverState::RUNNING;

        for (int i = 0; i < cores; ++i) {
            thread worker([this]() {
                while (currentState == SolverState::RUNNING) {
                    work();
                }
            });
            worker.join();
        }
    }

    void pause() {
        currentState = SolverState::PAUSED;
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
    Merger merger;
    OperationProducer operationProducer;
    SolverState currentState;

    set<Solution *> solutions;
    unsigned int cores;
    atomic<unsigned int> pos = 0;

    void work() {
        auto changer = pickChanger();
        auto bestSolution = solutions.begin();
        auto bestFormula = (*bestSolution)->getFormula();
        if(changer == nullptr) {
            advance(bestSolution, 1);
            auto secondBestFormula = (*bestSolution)->getFormula();
            auto formula = merger.merge(bestFormula, secondBestFormula);
            number rate = Evaluator::rate(formula, input, results);
            solutions.insert(make_shared<Solution>(formula, ChangerType::MERGER, rate).get());
        } else {
            auto formula = changer->change(bestFormula);
            number rate = Evaluator::rate(formula, input, results);
            solutions.insert(make_shared<Solution>(formula, changer->getType(), rate).get());
            if (rate > 0.9999999999) { // TODO: make this value configurable
                currentState = SolverState::DONE;
            }
        }
    }

    Changer *pickChanger() {
        auto it = solutions.begin();
        if (pos++ == cores) {
            pos = 0;
            if (merger.getCoin()->toss()) {
                return changerPicker.pickRandomChanger();
            } else if (solutions.size() > 1) {
                return nullptr;
            }            
        }
        
        advance(it, operationProducer.getRandomNumber()->calculate(0, min((int) solutions.size() - 1,
                                                                          (int) atomic_load(&pos))));
        Changer *changer = changerPicker.pickChanger((*it)->getLastChanger());
        return changer;
    }
};

#endif