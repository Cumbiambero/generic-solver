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

    bool operator<(const Solution &other) const { return rate < other.rate; }

    bool operator>(const Solution &other) const { return rate > other.rate; }

    bool operator<=(const Solution &other) const { return rate <= other.rate; }

    bool operator>=(const Solution &other) const { return rate >= other.rate; }

    bool operator==(const Solution &other) const {
        return lastChanger == other.lastChanger &&
               rate == other.rate &&
               formula.toString() == other.formula.toString();
    }

    bool operator!=(const Solution &other) const { return !(other == *this); }

    [[nodiscard]] Formula getFormula() const { return formula; }

    [[nodiscard]] ChangerType getLastChanger() const { return lastChanger; }

    [[nodiscard]] number getRate() const { return rate; }

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
                records++;
                number currentResult = formula.evaluate(input[l][c]);
                number expectedResult = expected[l][c];
                if (expectedResult == currentResult) {
                    result++;
                } else {
                    if (abs(expectedResult) != abs(currentResult)) {
                        auto dividend(min(expectedResult, currentResult));
                        auto divisor(max(expectedResult, currentResult));
                        if (divisor != 0) {
                            auto increment = abs(dividend / divisor);
                            if (increment != 0) {
                                result += increment < 1 && increment > 0 ? 1 - increment : 1 / increment;
                                continue;
                            }
                        }
                    }
                    result += result / records;
                }
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
            solutions.insert(Solution(formula, ChangerType::FLIPPER, Evaluator::rate(formula, input, results)));
        }
        currentState = SolverState::RUNNING;

        for (int i = 0; i < cores; ++i) {
            thread worker([this]() { work(); });
            worker.join();
        }
        if (currentState == SolverState::DONE) {
            cout << "\nFound: " << (*solutions.rbegin()).getFormula().toString() << endl;
            print();
            exit(0);
        }
    }

    void print() {
        cout.width(FORMULA_WIDTH);
        cout << left << "Formula:";
        cout.width(RATE_WIDTH);
        cout << right << "Rate:" << endl;
        cout.width(0);
        short counter = 0;
        for (auto revIt = solutions.rbegin(); revIt != solutions.rend() && counter < NUMBER_OF_RESULTS; revIt++) {
            cout.width(FORMULA_WIDTH);
            cout << left << revIt->getFormula().toString();
            cout.width(RATE_WIDTH);
            cout << right << fixed << setprecision(RATE_PRECISION) << revIt->getRate() << endl;
            ++counter;
        }
    }

    [[nodiscard]] SolverState getState() const {
        return currentState;
    }

    void shrink() {
        auto size = solutions.size();
        if (solutions.size() > SOLUTIONS_SIZE) {
            auto rBegin = solutions.rbegin();
            auto rEnd = solutions.rend();
            advance(rBegin, SOLUTIONS_SIZE);
            auto from = --rBegin.base();
            auto to = --rEnd.base();
            solutions.erase(from, to);
        }
        cout << solutions.size() - size << " solutions deleted.\n";
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
            auto reverseIterator = solutions.rbegin();
            auto bestFormula = (*reverseIterator).getFormula();
            number rate;
            if (changer == nullptr) {
                advance(reverseIterator, merger.getCoin()->toss() ? 1 : solutions.size() / 2);
                auto existingFormula = (*reverseIterator).getFormula();
                auto formula = merger.merge(bestFormula, existingFormula);
                rate = storeSolution(ChangerType::MERGER, formula);
            } else {
                auto formula = changer->change(bestFormula);
                rate = storeSolution(changer->getType(), formula);
            }
            if (rate > ALMOST_PERFECT) {
                currentState = SolverState::DONE;
            }
        }
    }

    number storeSolution(ChangerType type, Formula &formula) {
        lock.lock();
        auto rate = Evaluator::rate(formula, input, results);
        solutions.insert(Solution(formula, type, rate));
        lock.unlock();
        return rate;
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