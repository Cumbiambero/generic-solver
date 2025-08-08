#ifndef GENERIC_SOLVER_SOLVER_HPP
#define GENERIC_SOLVER_SOLVER_HPP

#include "solver-base.hpp"
#include "creators/operation-producer.hpp"
#include "changers/flipper.hpp"
#include "enhanced-fitness.hpp"
#include "ultra-precision-fitness.hpp"
#include <cmath>
#include <thread>
#include <utility>
#include <atomic>
#include <future>
#include <algorithm>
#include <execution>
#include <set>

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
        if (std::abs(a.rate_ - b.rate_) < EPSILON_FOR_RATE) {
            const auto aString = a.getFormula().toString();
            const auto bString = b.getFormula().toString();
            if (aString != bString) {
                return aString.size() > bString.size();
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
                switch (inputRow.size()) {
                    case 1: currentResult = mutableFormula.evaluate(inputRow[0]); break;
                    case 2: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1]); break;
                    case 3: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1], inputRow[2]); break;
                    case 4: currentResult = mutableFormula.evaluate(inputRow[0], inputRow[1], inputRow[2], inputRow[3]); break;
                    default: 
                        throw std::runtime_error("Too many variables for formula.evaluate");
                }
            } catch (...) {
                return 0.0L; // Any evaluation error results in zero fitness
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
           bool useEnhancedFitness = true)
            : variables_(std::move(variables)), 
              input_(std::move(input)), 
              results_(std::move(results)),
              cores_(std::max(1u, std::thread::hardware_concurrency() - 1)),
              currentState_(SolverState::READY),
              useEnhancedFitness_(useEnhancedFitness),
              enhancedEvaluator_(std::make_unique<EnhancedEvaluator>()) {}

    void start() {
        if (currentState_ == SolverState::READY) {
            auto node = operationProducer_.produce(variables_);
            Formula formula(node, variables_);
            const auto rate = useEnhancedFitness_ ? 
                enhancedEvaluator_->rate(formula, input_, results_) :
                Evaluator::rate(formula, input_, results_);
            
            std::unique_lock lock(solutionsMutex_);
            solutions_.emplace(std::move(formula), ChangerType::FLIPPER, rate);
            lock.unlock();
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
        
        if (currentState_ == SolverState::DONE) {
            print();
            std::exit(0);
        }
    }

    void print() const {
        std::cout << "\nMatches:\n";
        printSolutions(hallOfFame_);
        std::cout << "\nPrevious intentions:\n";
        printSolutions(solutions_);
    }

    static void printSolutions(const std::set<Solution>& target, 
                             std::size_t numberOfResults = NUMBER_OF_RESULTS) {
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
            std::cout << std::right << std::fixed 
                      << std::setprecision(RATE_PRECISION) << revIt->getRate() << std::endl;
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

    void shrink() {
        std::unique_lock lock(solutionsMutex_);
        const auto originalSize = solutions_.size();
        
        if (originalSize > SOLUTIONS_SIZE) {
            auto rBegin = solutions_.rbegin();
            std::advance(rBegin, SOLUTIONS_SIZE);
            auto from = rBegin.base();
            solutions_.erase(solutions_.begin(), from);
        }
        
        lock.unlock();
        std::cout << originalSize - solutions_.size() << " solutions deleted.\n";
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
    std::unique_ptr<EnhancedEvaluator> enhancedEvaluator_;

    void work() {
        std::size_t stagnationCounter = 0;
        std::size_t aggressiveStagnationCounter = 0;
        number lastBestRate = 0.0L;
        
        while (currentState_.load() == SolverState::RUNNING) {
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
            
            Formula newFormula = (changer == nullptr) 
                ? merger_.merge(bestFormula, existingFormula)
                : changer->change(merger_.getCoin()->toss() ? bestFormula : existingFormula);
                
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
        
        if (rate > ALMOST_PERFECT) {
            std::unique_lock hallLock(hallOfFameMutex_);
            hallOfFame_.insert(solution);
            if (hallOfFame_.size() >= HALL_OF_FAME_SIZE) {
                currentState_ = SolverState::DONE;
            }
            hallLock.unlock();
        }
        
        std::unique_lock solutionsLock(solutionsMutex_);
        solutions_.insert(std::move(solution));
        solutionsLock.unlock();
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
            Formula randomFormula(node, variables_);
            const auto rate = evaluateFormula(randomFormula);
            solutions_.emplace(std::move(randomFormula), ChangerType::FLIPPER, rate);
        }
        lock.unlock();
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
    }

    [[nodiscard]] number evaluateFormula(const Formula& formula) const {
        if (useEnhancedFitness_) {
            // Use ultra-precision for formulas that might be close to perfect
            number basicFitness = enhancedEvaluator_->rate(formula, input_, results_);
            
            // Switch to ultra-precision for high-fitness formulas
            if (basicFitness > 0.6L) {
                return UltraPrecisionEvaluator::rate(formula, input_, results_);
            }
            return basicFitness;
        } else {
            return Evaluator::rate(formula, input_, results_);
        }
    }

    [[nodiscard]] Changer* pickCreativeChanger() {
        // Bias towards creative/structural changers when stagnated
        auto creativeTypes = {
            ChangerType::SIMPLIFIER,
            ChangerType::FUNCTION_TRANSFORMER,
            ChangerType::VARIABLE_SWAPPER,
            ChangerType::STRUCTURE_MUTATOR,
            ChangerType::PURGER,
            ChangerType::ADAPTIVE_MUTATOR,
            ChangerType::FILTER_RELATIONSHIP_MUTATOR,
            ChangerType::EXPONENTIAL_PATTERN_ENHANCER,
            ChangerType::POWER_RELATIONSHIP_PROMOTER,
            ChangerType::PRECISION_TUNER,          // High-precision fine-tuning
            ChangerType::RANGE_OPTIMIZER,          // Range-aware optimization
            ChangerType::NONLINEARITY_INJECTOR     // Complex transformations
        };
        
        auto randomIndex = operationProducer_.getRandomNumber()->calculate(0, 11);
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
            return nullptr; // Use merger
        }
        
        auto it = solutions_.begin();
        const auto maxPos = std::min(static_cast<std::size_t>(solutions_.size() - 1), pos_.load());
        const auto randomPos = operationProducer_.getRandomNumber()->calculate(0, static_cast<int>(maxPos));
        std::advance(it, randomPos);
        const auto changerType = it->getLastChanger();
        lock.unlock();
        
        return changerPicker_.pickChanger(changerType);
    }
};

#endif