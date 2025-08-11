#include <iomanip>
#include <vector>
#include <limits>
#ifndef GENERIC_SOLVER_SOLVER_HPP
#define GENERIC_SOLVER_SOLVER_HPP

#include "solver-base.hpp"
#include "creators/operation-producer.hpp"
#include "changers/flipper.hpp"
#include "enhanced-fitness.hpp"
#include "ultra-precision-fitness.hpp"
#include "../tree/unary.hpp"
#include "../tree/binary.hpp"
#include <cmath>
#include <thread>
#include <utility>
#include <atomic>
#include <future>
#include <algorithm>
#include <execution>
#include <set>
#include <chrono>
#include <iomanip>
#include <vector>

class Solution {
public:
    Solution(Formula formula, ChangerType lastChanger, number rate) noexcept
            : formula_(std::move(formula)), lastChanger_(lastChanger), rate_(rate) {}

    Solution(const Solution& copy) = default;
    Solution(Solution&& other) noexcept = default;
    Solution& operator=(const Solution& other) = default;
    Solution& operator=(Solution&& other) noexcept = default;

    [[nodiscard]] bool operator<(const Solution& other) const noexcept {
        return compare(*this, other);
    }

    [[nodiscard]] static bool compare(const Solution& a, const Solution& b) noexcept {
        if (a.rate_ < b.rate_) {
            return true;
        }
        const auto diff = std::abs(a.rate_ - b.rate_);
        if (diff < EPSILON_FOR_RATE) {
            const auto aString = a.getFormula().toString();
            const auto bString = b.getFormula().toString();
            if (aString.size() != bString.size()) {
                // Prefer shorter formula when rates tie
                return aString.size() > bString.size();
            }
            // As a final tiebreaker, use lexicographical order to avoid set collisions
            if (aString != bString) {
                return aString > bString;
            }
        }
        return false;
    }

    [[nodiscard]] const Formula& getFormula() const noexcept { return formula_; }
    [[nodiscard]] ChangerType getLastChanger() const noexcept { return lastChanger_; }
    [[nodiscard]] number getRate() const noexcept { return rate_; }

private:
    Formula formula_;
    ChangerType lastChanger_;
    number rate_;
};

class Evaluator {
public:
    [[nodiscard]] static number rate(const Formula& formula, 
                                   const vector<vector<number>>& input, 
                                   const vector<vector<number>>& expected) {
        if (input.size() != expected.size()) {
            return 0.0L;
        }

        number result = 0.0L;
        const auto records = static_cast<number>(expected.size());
        bool allPerfect = true;
        
        // Create a non-const copy for evaluation
        Formula mutableFormula = formula;
        
        for (std::size_t i = 0; i < expected.size(); ++i) {
            const auto& inputRow = input[i];
            number currentResult;
            try {
                if (inputRow.size() <= 4) {
                    switch (inputRow.size()) {
                        case 1: currentResult = mutableFormula.evaluate(inputRow[0]); break;
                        case 2: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1]); break;
                        case 3: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1], inputRow[2]); break;
                        case 4: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1], inputRow[2], inputRow[3]); break;
                        default: currentResult = 0.0L; break;
                    }
                } else {
                    currentResult = mutableFormula.evaluate(inputRow);
                }
            } catch (...) {
                return 0.0L;
            }

            // Penalize any invalid result
            if (!std::isfinite(currentResult)) {
                return 0.0L;
            }

            const number expectedResult = expected[i][0];
            if (std::abs(expectedResult - currentResult) < EPSILON) {
                result += 1.0L;
            } else {
                allPerfect = false;
                result += calculatePartialFitness(currentResult, expectedResult);
            }
        }

        const number finalRate = result / records;
        return allPerfect ? 1.0L : std::min(finalRate, 0.999999L);
    }

private:
    [[nodiscard]] static number calculatePartialFitness(number current, number expected) noexcept {
        const auto absCurrent = std::abs(current);
        const auto absExpected = std::abs(expected);
        const auto dividend = std::min(absExpected, absCurrent);
        const auto divisor = std::max(absExpected, absCurrent);
        
        if (divisor < EPSILON) {
            return dividend < 1.0L ? 0.5L : 1.0L / dividend;
        }
        if (dividend < EPSILON) {
            return divisor < 1.0L ? 0.5L : 1.0L / divisor;
        }
        
        const number increment = dividend / divisor;
        return increment < 1.0L ? increment : 1.0L / increment;
    }
};

class Solver {
public:
    Solver(vector<Variable> variables, vector<vector<number>> input, vector<vector<number>> results, 
           bool useEnhancedFitness = true,
           bool useUltraPrecisionFitness = false,
           number targetFitness = ALMOST_PERFECT,
           std::size_t threads = 0,
           std::chrono::seconds timeLimit = std::chrono::seconds{0})
            : variables_(std::move(variables)), 
              input_(std::move(input)), 
              results_(std::move(results)),
              cores_(threads > 0 ? threads : std::max(1u, std::thread::hardware_concurrency() - 1)),
              currentState_(SolverState::READY),
              useEnhancedFitness_(useEnhancedFitness),
              useUltraPrecisionFitness_(useUltraPrecisionFitness),
              targetFitness_(targetFitness),
              enhancedEvaluator_(std::make_unique<EnhancedEvaluator>()) {
        if (timeLimit.count() > 0) {
            hasDeadline_ = true;
            deadline_ = std::chrono::steady_clock::now() + timeLimit;
        } else {
            hasDeadline_ = false;
        }
    }

    // Returns true if a solution meeting targetFitness_ (or perfect) was found
    bool start() {
        constexpr std::size_t INITIAL_POPULATION_SIZE = 20;
        if (currentState_ == SolverState::READY) {
            std::unique_lock lock(solutionsMutex_);
            for (std::size_t i = 0; i < INITIAL_POPULATION_SIZE; ++i) {
                auto node = operationProducer_.produce(variables_);
                if (!node) continue;
                Formula formula(node, variables_);
                const auto rate = useEnhancedFitness_ ? 
                    enhancedEvaluator_->rate(formula, input_, results_) :
                    Evaluator::rate(formula, input_, results_);
                solutions_.emplace(std::move(formula), ChangerType::FLIPPER, rate);
            }
            // Heuristic seeds (helps escape polynomial exponent stagnation)
            try {
                if (!variables_.empty()) {
                    auto varNode = std::make_shared<Variable>(variables_[0]);
                    // r^2
                    auto squareNode = std::make_shared<Square>(varNode);
                    auto piNode = std::make_shared<Pi>();
                    auto twoNode = std::make_shared<Number>(2.0L);
                    auto approxPiNode = std::make_shared<Number>(3.14159265358979323846L);

                    // pi * r^2
                    auto areaNode = std::make_shared<Multiplication>(piNode, squareNode);
                    // 2 * pi * r
                    auto circumNode = std::make_shared<Multiplication>(twoNode, std::make_shared<Multiplication>(piNode, varNode));
                    // approx constant * r^2 (alternative path)
                    auto approxAreaNode = std::make_shared<Multiplication>(approxPiNode, std::make_shared<Square>(varNode));

                    std::vector<NodePtr> heuristicRoots { areaNode, circumNode, approxAreaNode };
                    for (auto &root : heuristicRoots) {
                        Formula f(root, variables_);
                        const auto rate = useEnhancedFitness_ ? 
                            enhancedEvaluator_->rate(f, input_, results_) :
                            Evaluator::rate(f, input_, results_);
                        solutions_.emplace(std::move(f), ChangerType::SIMPLIFIER, rate);
                    }
                }
            } catch (...) {
                // Ignore heuristic seeding failures
            }
            lock.unlock();
            shrink();
        }
        
        currentState_ = SolverState::RUNNING;

        vector<std::thread> workers;
        workers.reserve(cores_);
        
        for (std::size_t i = 0; i < cores_; ++i) {
            workers.emplace_back([this]() { work(); });
        }
        
        for (auto& worker : workers) {
            worker.join();
        }
        
        bool success = false;
        if (currentState_ == SolverState::DONE) {
            {
                std::shared_lock hallLock(hallOfFameMutex_);
                for (const auto& sol : hallOfFame_) {
                    if (sol.getRate() >= targetFitness_ || isPerfectMatch(sol.getFormula())) {
                        success = true; break;
                    }
                }
            }
            print();
        }
        return success;
    }

    void print() const {
        std::cout << "\nMatches:\n";
        this->printSolutions(hallOfFame_);
        std::cout << "\nPrevious intentions:\n";
        this->printSolutions(solutions_);
    }
    
    void requestStop() noexcept {
        currentState_ = SolverState::DONE;
    }

    void printSolutions(const std::set<Solution>& target, 
                       std::size_t numberOfResults = NUMBER_OF_RESULTS) const {
        std::cout.width(FORMULA_WIDTH);
        std::cout << std::left << "Formula:";
        std::cout.width(RATE_WIDTH);
        std::cout << std::right << "Rate:" << std::endl;
        std::cout.width(0);
        
        std::size_t counter = 0;
        for (auto revIt = target.rbegin(); 
             revIt != target.rend() && counter < numberOfResults; 
             ++revIt, ++counter) {
            std::cout.width(FORMULA_WIDTH);
            std::cout << std::left << revIt->getFormula().toString();
            std::cout.width(RATE_WIDTH);
            std::cout << std::right << std::fixed << std::setprecision(RATE_PRECISION);
            if (isPerfectMatch(revIt->getFormula())) {
                std::cout << 1.0 << std::endl;
            } else {
                std::cout << revIt->getRate() << std::endl;
            }
        }
        
        // Print C++ code section
        if (!target.empty()) {
            std::cout << "\nC++ Code (for integration):" << std::endl;
            counter = 0;
            for (auto revIt = target.rbegin(); 
                 revIt != target.rend() && counter < numberOfResults; 
                 ++revIt, ++counter) {
                std::cout << "// Formula " << (counter + 1) << ": " 
                          << revIt->getFormula().toString() << std::endl;
                std::cout << "long double result = " 
                          << revIt->getFormula().toCppCode() << ";" << std::endl;
                std::cout << std::endl;
            }
        }
    }

    [[nodiscard]] SolverState getState() const noexcept {
        return currentState_.load();
    }

    void shrinkNoPrintNoUnlock() {
        if (solutions_.size() > SOLUTIONS_SIZE) {
            auto rBegin = solutions_.rbegin();
            std::advance(rBegin, SOLUTIONS_SIZE);
            auto from = rBegin.base();
            solutions_.erase(solutions_.begin(), from);
        }
    }

    void shrink() {
        std::unique_lock lock(solutionsMutex_);
        shrinkNoPrintNoUnlock();
    }

private:
    const vector<Variable> variables_;
    const vector<vector<number>> input_;
    const vector<vector<number>> results_;
    
    mutable std::shared_mutex solutionsMutex_;
    mutable std::shared_mutex hallOfFameMutex_;
    
    ChangerPicker changerPicker_;
    Merger merger_;
    OperationProducer operationProducer_;
    
    std::atomic<std::size_t> cores_;
    std::atomic<SolverState> currentState_;
    std::atomic<std::size_t> pos_{0};

    std::set<Solution> solutions_;
    std::set<Solution> hallOfFame_;
    
    bool useEnhancedFitness_;
    bool useUltraPrecisionFitness_;
    number targetFitness_;
    std::unique_ptr<EnhancedEvaluator> enhancedEvaluator_;
    
    // Early stopping controls
    std::atomic<bool> hasDeadline_{false};
    std::chrono::steady_clock::time_point deadline_{};

    void work() {
        std::size_t stagnationCounter = 0;
        std::size_t aggressiveStagnationCounter = 0;
        number lastBestRate = 0.0L;
        std::size_t iterations = 0;
        constexpr std::size_t ITERATION_HARD_CAP = 200000;
        
        while (currentState_.load() == SolverState::RUNNING) {
            if (++iterations > ITERATION_HARD_CAP) {
                currentState_ = SolverState::DONE;
                break;
            }
            // Check deadline
            if (hasDeadline_.load()) {
                if (std::chrono::steady_clock::now() >= deadline_) {
                    currentState_ = SolverState::DONE;
                    break;
                }
            }
            auto changer = pickChanger();
            
            // Adaptive mutation: prefer more creative changers when stagnated
            if (aggressiveStagnationCounter > AGGRESSIVE_STAGNATION_THRESHOLD) {
                changer = pickCreativeChanger();
            }
            
            std::shared_lock readLock(solutionsMutex_);
            if (solutions_.empty()) {
                readLock.unlock();
                continue;
            }
            
            const Formula bestFormula = solutions_.rbegin()->getFormula();
            const auto shift = merger_.getCoin()->toss() ? 1 : solutions_.size() >> 1;
            auto reverseIterator = solutions_.rbegin();
            std::advance(reverseIterator, std::min(shift, solutions_.size() - 1));
            const Formula existingFormula = reverseIterator->getFormula();
            readLock.unlock();
            
            Formula newFormula = bestFormula;
            try {
                if (changer == nullptr) {
                    newFormula = merger_.merge(bestFormula, existingFormula);
                } else {
                    newFormula = changer->change(merger_.getCoin()->toss() ? bestFormula : existingFormula);
                }
            } catch (...) {
                continue; // skip failed mutation
            }
                
            const auto changerType = changer ? changer->getType() : ChangerType::MERGER;
            storeSolution(changerType, newFormula);

            // Enhanced stagnation detection and handling
            std::shared_lock bestRateReadLock(solutionsMutex_);
            const number currentBestRate = solutions_.empty() ? 0.0L : solutions_.rbegin()->getRate();
            bestRateReadLock.unlock();
            
            if (std::abs(currentBestRate - lastBestRate) < 1e-8L) {
                ++stagnationCounter;
                ++aggressiveStagnationCounter;
            } else {
                stagnationCounter = 0;
                aggressiveStagnationCounter = 0;
                lastBestRate = currentBestRate;
            }
            
            // Stop if target fitness achieved or perfect match found
            if (currentBestRate >= targetFitness_) {
                currentState_ = SolverState::DONE;
                break;
            }
            // Extra: stop if any solution is a perfect match (snapshot under lock)
            {
                std::shared_lock anyLock(solutionsMutex_);
                for (const auto& sol : solutions_) {
                    if (isPerfectMatch(sol.getFormula())) {
                        currentState_ = SolverState::DONE;
                        break;
                    }
                }
            }
            
            // Progressive intervention when stagnated
            if (stagnationCounter > STAGNATION_THRESHOLD) {
                injectRandomness();
                stagnationCounter = 0;
            }
            
            // More aggressive intervention for prolonged stagnation
            if (aggressiveStagnationCounter > AGGRESSIVE_STAGNATION_THRESHOLD * 2) {
                injectCreativeRandomness();
                aggressiveStagnationCounter = 0;
            }
        }
    }

    void storeSolution(ChangerType type, const Formula& formula) {
        const auto rate = evaluateFormula(formula);
        Solution solution(formula, type, rate);
        
        if (rate >= targetFitness_ || isPerfectMatch(formula)) {
            std::unique_lock hallLock(hallOfFameMutex_);
            hallOfFame_.insert(solution);
            currentState_ = SolverState::DONE;
            hallLock.unlock();
        }
        
        if (rate > ALMOST_PERFECT) {
            std::unique_lock hallLock(hallOfFameMutex_);
            hallOfFame_.insert(solution);
            if (hallOfFame_.size() >= HALL_OF_FAME_SIZE) {
                currentState_ = SolverState::DONE;
            }
            hallLock.unlock();
        }
        
        {
            std::unique_lock solutionsLock(solutionsMutex_);
            solutions_.insert(std::move(solution));
            if (solutions_.size() > SOLUTIONS_SIZE) {
                shrinkNoPrintNoUnlock();
            }
        }
    }

    void injectRandomness() {
        std::unique_lock lock(solutionsMutex_);
        if (solutions_.empty()) {
            lock.unlock();
            return;
        }
        
        const Solution best = *solutions_.rbegin();
        solutions_.clear();
        solutions_.insert(best);
        
        for (std::size_t i = 0; i < RANDOM_INJECTION_COUNT; ++i) {
            auto node = operationProducer_.produce(variables_);
            if (!node) continue;
            try {
                Formula randomFormula(node, variables_);
                const auto rate = evaluateFormula(randomFormula);
                solutions_.emplace(std::move(randomFormula), ChangerType::FLIPPER, rate);
            } catch (...) {
                // skip bad random formula
            }
        }
    lock.unlock();
    shrink();
    }

    void injectCreativeRandomness() {
        std::unique_lock lock(solutionsMutex_);
        if (solutions_.empty()) {
            lock.unlock();
            return;
        }
        
        // Keep top 3 solutions and create variations using creative operators
        std::vector<Solution> topSolutions;
        auto it = solutions_.rbegin();
        for (int i = 0; i < 3 && it != solutions_.rend(); ++i, ++it) {
            topSolutions.push_back(*it);
        }
        
        solutions_.clear();
        
        // Re-insert top solutions
        for (const auto& sol : topSolutions) {
            solutions_.insert(sol);
        }
        
        // Create variations using creative operators
        auto creativeChangers = {
            changerPicker_.pickChanger(ChangerType::SIMPLIFIER),
            changerPicker_.pickChanger(ChangerType::FUNCTION_TRANSFORMER),
            changerPicker_.pickChanger(ChangerType::VARIABLE_SWAPPER),
            changerPicker_.pickChanger(ChangerType::STRUCTURE_MUTATOR)
        };
        
        for (const auto& baseSolution : topSolutions) {
            for (auto changer : creativeChangers) {
                if (changer) {
                    try {
                        auto newFormula = changer->change(baseSolution.getFormula());
                        const auto rate = evaluateFormula(newFormula);
                        solutions_.emplace(std::move(newFormula), changer->getType(), rate);
                    } catch (...) {
                        // Skip if changer fails
                    }
                }
            }
        }
        
    lock.unlock();
    shrink();
    }

    [[nodiscard]] number evaluateFormula(const Formula& formula) const {
        if (useUltraPrecisionFitness_) {
            return UltraPrecisionEvaluator::rate(formula, input_, results_);
        }
        return useEnhancedFitness_ ? 
            enhancedEvaluator_->rate(formula, input_, results_) :
            Evaluator::rate(formula, input_, results_);
    }

    [[nodiscard]] Changer* pickCreativeChanger() {
        auto creativeTypes = {
            ChangerType::TARGETED_TUNER,
            ChangerType::PATTERN_OPTIMIZER,
            ChangerType::SIMPLIFIER,
            ChangerType::FUNCTION_TRANSFORMER,
            ChangerType::VARIABLE_SWAPPER,
            ChangerType::STRUCTURE_MUTATOR,
            ChangerType::ADAPTIVE_MUTATOR,
            ChangerType::FILTER_RELATIONSHIP_MUTATOR,
            ChangerType::EXPONENTIAL_PATTERN_ENHANCER,
            ChangerType::POWER_RELATIONSHIP_PROMOTER
        };
        
        auto randomIndex = operationProducer_.getRandomNumber()->calculate(0, 9);
        auto selectedType = *std::next(creativeTypes.begin(), randomIndex);
        
        return changerPicker_.pickChanger(selectedType);
    }

    [[nodiscard]] Changer* pickChanger() {
        std::shared_lock lock(solutionsMutex_);
        if (solutions_.empty()) {
            lock.unlock();
            return changerPicker_.pickRandomChanger();
        }
        
        if (merger_.getCoin()->toss()) {
            lock.unlock();
            return changerPicker_.pickRandomChanger();
        }
        
        if (solutions_.size() <= 1) {
            lock.unlock();
            return nullptr;
        }
        
        auto it = solutions_.begin();
        const auto maxPos = std::min(static_cast<std::size_t>(solutions_.size() - 1), pos_.load());
        const auto randomPos = operationProducer_.getRandomNumber()->calculate(0, static_cast<int>(maxPos));
        std::advance(it, randomPos);
        const auto changerType = it->getLastChanger();
        lock.unlock();
        
        return changerPicker_.pickChanger(changerType);
    }

private:
    bool isPerfectMatch(const Formula& formula) const {
        if (input_.size() != results_.size()) return false;
        Formula mutableFormula = formula;
        for (std::size_t i = 0; i < results_.size(); ++i) {
            const auto& inputRow = input_[i];
            number currentResult;
            try {
                if (inputRow.size() <= 4) {
                    switch (inputRow.size()) {
                        case 1: currentResult = mutableFormula.evaluate(inputRow[0]); break;
                        case 2: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1]); break;
                        case 3: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1], inputRow[2]); break;
                        case 4: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1], inputRow[2], inputRow[3]); break;
                        default: currentResult = 0.0L; break;
                    }
                } else {
                    currentResult = mutableFormula.evaluate(inputRow);
                }
            } catch (...) {
                return false;
            }
            if (!std::isfinite(currentResult)) return false;
            const number expectedResult = results_[i][0];
            if (std::abs(expectedResult - currentResult) >= EPSILON) return false;
        }
        return true;
    }
};

#endif