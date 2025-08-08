#ifndef GENERIC_SOLVER_ULTRA_PRECISION_FITNESS_HPP
#define GENERIC_SOLVER_ULTRA_PRECISION_FITNESS_HPP

#include "formula.hpp"
#include "../utils/base.hpp"
#include "../utils/config.hpp"
#include <cmath>
#include <vector>
#include <limits>

class UltraPrecisionEvaluator {
public:
    [[nodiscard]] static number rate(const Formula& formula, 
                                   const vector<vector<number>>& input, 
                                   const vector<vector<number>>& expected) {
        if (input.size() != expected.size()) {
            return 0.0L;
        }

        number accuracyFitness = calculateUltraAccuracyFitness(formula, input, expected);
        number complexityPenalty = calculateGentleComplexityPenalty(formula);
        number rangeAdaptation = calculateRangeAdaptation(formula, input, expected);
        number precisionBonus = calculatePrecisionBonus(formula, input, expected);

        const number accuracyWeight = 0.85L;     // Increased from 0.7
        const number complexityWeight = 0.05L;   // Reduced from 0.1
        const number rangeWeight = 0.05L;        // Reduced from 0.1
        const number precisionWeight = 0.05L;    // New precision component
        
        number finalFitness = accuracyWeight * accuracyFitness +
                             complexityWeight * (1.0L - complexityPenalty) +
                             rangeWeight * rangeAdaptation +
                             precisionWeight * precisionBonus;
        
        return std::min(finalFitness, 0.999999L);
    }

private:
    [[nodiscard]] static number calculateUltraAccuracyFitness(const Formula& formula,
                                                             const vector<vector<number>>& input,
                                                             const vector<vector<number>>& expected) {
        number result = 0.0L;
        const auto records = static_cast<number>(expected.size());
        
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

            if (!std::isfinite(currentResult)) {
                return 0.0L;
            }

            const number expectedResult = expected[i][0];
            
            if (std::abs(expectedResult - currentResult) < 1e-10L) {
                result += 1.0L; // Perfect match
            } else {
                number partialFitness = calculateUltraPartialFitness(currentResult, expectedResult);
                result += partialFitness;
            }
        }

        return result / records;
    }

    [[nodiscard]] static number calculateUltraPartialFitness(number current, number expected) noexcept {
        if (std::abs(expected) < 1e-10L && std::abs(current) < 1e-10L) {
            return 1.0L; // Both near zero
        }
        
        number relativeError, absoluteError;
        
        if (std::abs(expected) > 1.0L) {
            relativeError = std::abs((current - expected) / expected);
            return std::max(0.0L, 1.0L - relativeError);
        } else {
            absoluteError = std::abs(current - expected);
            return std::max(0.0L, 1.0L - absoluteError);
        }
    }

    [[nodiscard]] static number calculateGentleComplexityPenalty(const Formula& formula) {
        const string formulaStr = formula.toString();
        const size_t length = formulaStr.length();
        
        if (length > 300) {
            return 0.1L; // 10% penalty for very complex formulas
        } else if (length > 200) {
            return 0.05L; // 5% penalty for moderately complex formulas
        }
        return 0.0L;
    }

    [[nodiscard]] static number calculateRangeAdaptation(const Formula& formula,
                                                        const vector<vector<number>>& input,
                                                        const vector<vector<number>>& expected) {
        Formula mutableFormula = formula;
        
        number minExpected = std::numeric_limits<number>::max();
        number maxExpected = std::numeric_limits<number>::lowest();
        number minPredicted = std::numeric_limits<number>::max();
        number maxPredicted = std::numeric_limits<number>::lowest();
        
        for (std::size_t i = 0; i < expected.size(); ++i) {
            number expectedVal = expected[i][0];
            minExpected = std::min(minExpected, expectedVal);
            maxExpected = std::max(maxExpected, expectedVal);
            
            try {
                const auto& inputRow = input[i];
                number predicted;
                if (inputRow.size() <= 4) {
                    switch (inputRow.size()) {
                        case 1: predicted = mutableFormula.evaluate(inputRow[0]); break;
                        case 2: predicted = mutableFormula.evaluate(inputRow[0], inputRow[1]); break;
                        case 3: predicted = mutableFormula.evaluate(inputRow[0], inputRow[1], inputRow[2]); break;
                        case 4: predicted = mutableFormula.evaluate(inputRow[0], inputRow[1], inputRow[2], inputRow[3]); break;
                        default: continue;
                    }
                } else {
                    predicted = mutableFormula.evaluate(inputRow);
                }
                
                if (std::isfinite(predicted)) {
                    minPredicted = std::min(minPredicted, predicted);
                    maxPredicted = std::max(maxPredicted, predicted);
                }
            } catch (...) {
                // Skip invalid predictions
            }
        }
        
        if (!std::isfinite(minPredicted) || !std::isfinite(maxPredicted)) {
            return 0.0L;
        }
        
        number expectedRange = maxExpected - minExpected;
        number predictedRange = maxPredicted - minPredicted;
        
        if (expectedRange < 1e-10L || predictedRange < 1e-10L) {
            return 0.5L;
        }
        
        number rangeFactor = std::min(expectedRange, predictedRange) / std::max(expectedRange, predictedRange);
        return rangeFactor;
    }

    [[nodiscard]] static number calculatePrecisionBonus(const Formula& formula,
                                                       const vector<vector<number>>& input,
                                                       const vector<vector<number>>& expected) {
        number nearPerfectCount = 0.0L;
        const auto totalCount = static_cast<number>(expected.size());
        
        Formula mutableFormula = formula;
        
        for (std::size_t i = 0; i < expected.size(); ++i) {
            try {
                const auto& inputRow = input[i];
                number predicted;
                switch (inputRow.size()) {
                    case 1: predicted = mutableFormula.evaluate(inputRow[0]); break;
                    case 2: predicted = mutableFormula.evaluate(inputRow[0], inputRow[1]); break;
                    default: continue;
                }
                
                number expectedVal = expected[i][0];
                number error = std::abs(predicted - expectedVal);
                
                number tolerance = std::max(0.01L * std::abs(expectedVal), 0.01L);
                if (error < tolerance) {
                    nearPerfectCount += 1.0L;
                }
            } catch (...) {
                // Skip invalid predictions
            }
        }
        
        return nearPerfectCount / totalCount;
    }
};

#endif
