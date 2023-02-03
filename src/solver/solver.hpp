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

    bool operator<(const Solution &other) const {
        return compare(this, &other);
    }

    static bool compare(const Solution *a, const Solution *b) {
        if (a->rate < b->rate) {
            return true;
        } else if (std::fabs(a->rate - b->rate) < EPSILON_FOR_RATE) {
            auto aString = a->getFormula().toString();
            auto bString = b->getFormula().toString();
            if (aString != bString) {
                return aString.size() > bString.size();
            }
        }
        return false;
    }

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
        for (auto l = 0; l < expected.size(); l++) {
            for (auto c = 0; c < expected[l].size(); c++) {
                ++records;
                number currentResult = formula.evaluate(input[l][c]);
                number expectedResult = expected[l][c];
                if (expectedResult == currentResult) {
                    ++result;
                } else {
                    number increment;
                    auto absCurrent = abs(currentResult);
                    auto absExpected = abs(expectedResult);
                    auto dividend(min(absExpected, absCurrent));
                    auto divisor(max(absExpected, absCurrent));
                    if (divisor == 0) {
                        increment = dividend == 1 ? 0.5 : 1 / dividend;
                    } else if (dividend == 0) {
                        increment = divisor == 1 ? 0.5 : 1 / divisor;
                    } else {
                        increment = dividend / divisor;
                    }
                    result += increment < 1 ? increment : 1 / increment;
                }
            }
        }
        return result / records;
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

        for (auto i = 0; i < cores; ++i) {
            thread worker([this]() { work(); });
            worker.join();
        }
        if (currentState == SolverState::DONE) {
            print();
            exit(0);
        }
    }

    void print() {
        cout << "\nMatches:\n";
        printSolutions(hallOfFame);
        cout << "\nPrevious intentions:\n";
        printSolutions(solutions);
    }

    static void printSolutions(set<Solution> &target, short numberOfResults = NUMBER_OF_RESULTS) {
        cout.width(FORMULA_WIDTH);
        cout << left << "Formula:";
        cout.width(RATE_WIDTH);
        cout << right << "Rate:" << endl;
        cout.width(0);
        short counter = 0;
        for (auto revIt = target.rbegin(); revIt != target.rend() && counter < numberOfResults; revIt++) {
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
    set<Solution> hallOfFame;
    unsigned int cores;
    atomic<unsigned int> pos = 0;

    void work() {
        while (currentState == SolverState::RUNNING) {
            auto changer = pickChanger();
            auto reverseIterator = solutions.rbegin();
            Formula bestFormula((*reverseIterator).getFormula());
            auto shift = merger.getCoin()->toss() ? 1 : solutions.size() >> 1;
            advance(reverseIterator, shift);
            Formula existingFormula((*reverseIterator).getFormula());
            if (changer == nullptr) {
                auto formula = merger.merge(bestFormula, existingFormula);
                storeSolution(ChangerType::MERGER, formula);
            } else {
                auto formula = changer->change(merger.getCoin()->toss() ? bestFormula : existingFormula);
                storeSolution(changer->getType(), formula);
            }
        }
    }

    void storeSolution(ChangerType type, Formula &formula) {
        lock.lock();
        auto rate = Evaluator::rate(formula, input, results);
        auto solution = Solution(formula, type, rate);
        if (rate > ALMOST_PERFECT) {
            hallOfFame.insert(solution);
            if (hallOfFame.size() >= HALL_OF_FAME_SIZE) {
                currentState = SolverState::DONE;
            }
        }
        solutions.insert(solution);
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