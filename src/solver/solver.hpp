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
    Solution(const Formula &formula, ChangerType lastChanger, number rate)
            : formula(formula), lastChanger(lastChanger), rate(rate) {}

    Solution(const Solution &copy) = default;

    bool operator<(const Solution &other) const { return (this->rate < other.rate); }

    bool operator>(const Solution &other) const {
        return rate > other.rate;
    }

    bool operator<=(const Solution &other) const {
        return rate <= other.rate;
    }

    bool operator>=(const Solution &other) const {
        return rate >= other.rate;
    }

    bool operator==(const Solution &other) const {
        return lastChanger == other.lastChanger &&
               rate == other.rate &&
               formula.toString() == other.formula.toString();
    }

    bool operator!=(const Solution &other) const {
        return !(other == *this);
    }

    [[nodiscard]] Formula getFormula() const { return formula; }

    [[nodiscard]] ChangerType getLastChanger() const { return lastChanger; }

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
                result += abs(dividend / divisor);
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
              cores(max((unsigned int) 1, thread::hardware_concurrency() - 1)),
              currentState(SolverState::READY) {}

    void start() {
        if (currentState == SolverState::READY) {
            auto node = operationProducer.produce(variables);
            Formula formula(node, variables);
            solutions.insert(Solution(formula, ChangerType::OPERATION_REPLACER, 0.0));
        }
        currentState = SolverState::RUNNING;

        for (int i = 0; i < cores; ++i) {
            thread worker([this]() { work(); });
            worker.join();
        }
        if (currentState == SolverState::DONE) {
            cout << "\nFound: " << (*solutions.rbegin()).getFormula().toString() << endl;
            exit(0);
        }
    }

    void print() {
        cout.width(70);
        cout << left << "Formula:";
        cout.width(9);
        cout << left << "Rate:" << endl;
        cout.width(0);
        for (auto solution: solutions) {
            cout.width(70);
            cout << left << solution.getFormula().toString();
            cout.width(9);
            cout << right << fixed << setprecision(7) << solution.getRate() << endl;
        }
    }

    [[nodiscard]] SolverState getState() const {
        return currentState;
    }

private:
    const vector<Variable> variables;
    const vector<vector<number>> input;
    const vector<vector<number>> results;
    mutex lock;
    ChangerPicker changerPicker;
    Merger merger;
    OperationProducer operationProducer;
    SolverState currentState;

    set<Solution> solutions;
    unsigned int cores;
    atomic<unsigned int> pos = 0;

    void work() {
        while (currentState == SolverState::RUNNING) {
            auto changer = pickChanger();
            auto bestSolutionIt = solutions.begin();
            auto bestFormula = (*bestSolutionIt).getFormula();
            number rate;
            if (changer == nullptr) {
                advance(bestSolutionIt, 1);
                auto secondBestFormula = (*bestSolutionIt).getFormula();
                auto formula = merger.merge(bestFormula, secondBestFormula);
                rate = Evaluator::rate(formula, input, results);
                storeSolution(ChangerType::MERGER, rate, formula);
            } else {
                auto formula = changer->change(bestFormula);
                rate = Evaluator::rate(formula, input, results);
                storeSolution(changer->getType(), rate, formula);
            }
            if (rate > ALMOST_PERFECT) {
                currentState = SolverState::DONE;
            }
        }
    }

    void storeSolution(ChangerType type, number rate, const Formula &formula) {
        lock.lock();
        solutions.insert(Solution(formula, type, rate));
        lock.unlock();
    }

    Changer *pickChanger() {
        auto it = solutions.begin();
        if (merger.getCoin()->toss()) {
            return changerPicker.pickRandomChanger();
        } else if (solutions.size() > 1) {
            return nullptr;
        }

        advance(it, operationProducer.getRandomNumber()->calculate(0, min((int) solutions.size() - 1,
                                                                          (int) atomic_load(&pos))));
        Changer *changer = changerPicker.pickChanger((*it).getLastChanger());
        return changer;
    }
};

#endif