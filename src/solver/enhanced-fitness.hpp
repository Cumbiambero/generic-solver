#ifndef GENERIC_SOLVER_ENHANCED_FITNESS_HPP
#define GENERIC_SOLVER_ENHANCED_FITNESS_HPP

#include "solver.hpp"

class EnhancedEvaluator {
public:
    [[nodiscard]] static number rate(const Formula& formula, 
                                   const vector<vector<number>>& input, 
                                   const vector<vector<number>>& expected) {
        if (input.size() != expected.size()) {
            return 0.0L;
        }

        number accuracyFitness = calculateAccuracyFitness(formula, input, expected);
        number complexityPenalty = calculateComplexityPenalty(formula);
        number rangeConsistency = calculateRangeConsistency(formula, input, expected);
        number monotonicityBonus = calculateMonotonicityBonus(formula, input, expected);
        
        const number baseWeight = 0.7L;         // Accuracy is most important
        const number complexityWeight = 0.1L;   // Prefer simpler formulas
        const number rangeWeight = 0.1L;        // Stay within expected bounds
        const number monotonicityWeight = 0.1L; // Prefer smooth relationships
        
        number finalFitness = baseWeight * accuracyFitness +
                             complexityWeight * (1.0L - complexityPenalty) +
                             rangeWeight * rangeConsistency +
                             monotonicityWeight * monotonicityBonus;
        
        return std::min(finalFitness, 0.999999L);
    }

private:
    [[nodiscard]] static number calculateAccuracyFitness(const Formula& formula,
                                                        const vector<vector<number>>& input,
                                                        const vector<vector<number>>& expected) {
        number result = 0.0L;
        const auto records = static_cast<number>(expected.size());
        bool allPerfect = true;
        
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
                return 0.0L;
            }

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
        return allPerfect ? 1.0L : finalRate;
    }

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

    [[nodiscard]] static number calculateComplexityPenalty(const Formula& formula) {
        const string formulaStr = formula.toString();
        const size_t length = formulaStr.length();
        const auto operationCount = std::count_if(formulaStr.begin(), formulaStr.end(), 
            [](char c) { return c == '+' || c == '-' || c == '*' || c == '/' || c == '^'; });
        const size_t operations = static_cast<size_t>(std::max(0L, operationCount));
        
        const number lengthPenalty = std::min(1.0L, static_cast<number>(length) / 200.0L);
        const number operationPenalty = std::min(1.0L, static_cast<number>(operations) / 20.0L);
        
        return (lengthPenalty + operationPenalty) / 2.0L;
    }

    [[nodiscard]] static number calculateRangeConsistency(const Formula& formula,
                                                         const vector<vector<number>>& input,
                                                         const vector<vector<number>>& expected) {
        number minExpected = expected[0][0], maxExpected = expected[0][0];
        for (const auto& row : expected) {
            minExpected = std::min(minExpected, row[0]);
            maxExpected = std::max(maxExpected, row[0]);
        }
        
        const number expectedRange = maxExpected - minExpected;
        if (expectedRange < EPSILON) return 1.0L;
        
        Formula mutableFormula = formula;
        number minOutput = std::numeric_limits<number>::max();
        number maxOutput = std::numeric_limits<number>::lowest();
        
        for (const auto& inputRow : input) {
            try {
                number output;
                switch (inputRow.size()) {
                    case 1: output = mutableFormula.evaluate(inputRow[0]); break;
                    case 2: output = mutableFormula.evaluate(inputRow[0], inputRow[1]); break;
                    default: continue;
                }
                
                if (std::isfinite(output)) {
                    minOutput = std::min(minOutput, output);
                    maxOutput = std::max(maxOutput, output);
                }
            } catch (...) {
                return 0.0L;
            }
        }
        
        const number outputRange = maxOutput - minOutput;
        
        const number rangeDifference = std::abs(outputRange - expectedRange);
        return std::max(0.0L, 1.0L - (rangeDifference / expectedRange));
    }

    [[nodiscard]] static number calculateMonotonicityBonus(const Formula& formula,
                                                          const vector<vector<number>>& input,
                                                          const vector<vector<number>>& expected) {
        if (input.size() < 2 || input[0].size() == 0) return 0.0L;
        
        Formula mutableFormula = formula;
        
        number monotonicityScore = 0.0L;
        size_t validComparisons = 0;
        
        for (size_t i = 1; i < input.size(); ++i) {
            try {
                number output1, output2;
                switch (input[i-1].size()) {
                    case 1: output1 = mutableFormula.evaluate(input[i-1][0]); break;
                    case 2: output1 = mutableFormula.evaluate(input[i-1][0], input[i-1][1]); break;
                    default: continue;
                }
                
                switch (input[i].size()) {
                    case 1: output2 = mutableFormula.evaluate(input[i][0]); break;
                    case 2: output2 = mutableFormula.evaluate(input[i][0], input[i][1]); break;
                    default: continue;
                }
                
                if (!std::isfinite(output1) || !std::isfinite(output2)) continue;
                
                const number expectedTrend = expected[i][0] - expected[i-1][0];
                const number actualTrend = output2 - output1;
                
                if ((expectedTrend > 0 && actualTrend > 0) || 
                    (expectedTrend < 0 && actualTrend < 0) ||
                    (std::abs(expectedTrend) < EPSILON && std::abs(actualTrend) < EPSILON)) {
                    monotonicityScore += 1.0L;
                }
                
                validComparisons++;
                
            } catch (...) {
                continue;
            }
        }
        
        return validComparisons > 0 ? monotonicityScore / validComparisons : 0.0L;
    }
};

#endif
