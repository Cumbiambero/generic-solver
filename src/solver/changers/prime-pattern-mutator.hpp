#ifndef GENERIC_SOLVER_PRIME_PATTERN_MUTATOR_HPP
#define GENERIC_SOLVER_PRIME_PATTERN_MUTATOR_HPP

#include "changer-base.hpp"
#include "../../utils/base.hpp"

class PrimePatternMutator final : public Changer {
public:
    PrimePatternMutator() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::PRIME_PATTERN_MUTATOR;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                const auto primeRelatedConstants = {
                    2.0L,           
                    3.0L,           
                    5.0L,           
                    6.0L,           
                    7.0L,           
                    11.0L,          
                    13.0L,          
                    1.0L,           
                    0.5L,           
                    0.333333333L,   
                    0.166666666L,   
                    2.302585093L,   
                    1.442695041L,   
                    0.915965594L,   
                    1.781072418L,   
                    0.261497212L    
                };
                
                auto randomIndex = randomNumber_.calculate(0, static_cast<int>(primeRelatedConstants.size()) - 1);
                auto primeConstant = *std::next(primeRelatedConstants.begin(), randomIndex);
                
                if (coin_->toss()) {
                    number->setValue(primeConstant);
                } else {
                    if (std::abs(currentValue) > EPSILON) {
                        number->setValue(currentValue * primeConstant);
                    } else {
                        number->setValue(primeConstant);
                    }
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

class ModularArithmeticEnhancer final : public Changer {
public:
    ModularArithmeticEnhancer() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::MODULAR_ARITHMETIC_ENHANCER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                const auto modularBases = {
                    2.0L, 3.0L, 5.0L, 6.0L, 7.0L, 10.0L, 12.0L
                };
                
                auto randomIndex = randomNumber_.calculate(0, static_cast<int>(modularBases.size()) - 1);
                auto base = *std::next(modularBases.begin(), randomIndex);
                
                if (coin_->toss() && std::abs(currentValue) > 1.0L) {
                    number->setValue(std::fmod(currentValue, base));
                } else {
                    number->setValue(base);
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

class PolynomialCoefficientTuner final : public Changer {
public:
    PolynomialCoefficientTuner() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::POLYNOMIAL_COEFFICIENT_TUNER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                const auto polynomialCoefficients = {
                    0.0L, 1.0L, -1.0L,
                    2.0L, -2.0L, 0.5L, -0.5L,
                    3.0L, -3.0L, 0.333333333L, -0.333333333L,
                    4.0L, -4.0L, 0.25L, -0.25L,
                    6.0L, -6.0L, 0.166666666L
                };
                
                auto idx = randomNumber_.calculate(0, static_cast<int>(polynomialCoefficients.size()) - 1);
                auto coefficient = *std::next(polynomialCoefficients.begin(), idx);
                
                if (coin_->toss()) {
                    number->setValue(coefficient);
                } else if (std::abs(currentValue) > EPSILON) {
                    auto adjustment = randomNumber_.calculate(-5, 5) * 0.01L;
                    number->setValue(currentValue + adjustment);
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

#endif
