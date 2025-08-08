#ifndef GENERIC_SOLVER_AUDIO_OPERATORS_HPP
#define GENERIC_SOLVER_AUDIO_OPERATORS_HPP

#include "changer-base.hpp"

class FilterRelationshipMutator final : public Changer {
public:
    FilterRelationshipMutator() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::FILTER_RELATIONSHIP_MUTATOR;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto value = number->calculate();
                
                if (std::abs(value) > 1e6L) {
                    const auto audioConstants = {
                        1.0L, 2.0L, 0.5L,        // Basic ratios
                        0.707L,                   // -3dB point (sqrt(2)/2)
                        3.14159L, 6.28318L,      // π, 2π (frequency relationships)
                        20.0L, 20000.0L,         // Human hearing range
                        440.0L, 1000.0L,         // Reference frequencies
                        0.1L, 0.01L,             // Small scaling factors
                        12.0L, 24.0L,            // Octave relationships
                        1.414L,                  // sqrt(2) - octave ratio
                        2.718L                   // e - exponential decay
                    };
                    
                    auto randomIndex = randomNumber_.calculate(0, static_cast<int>(audioConstants.size()) - 1);
                    auto newValue = *std::next(audioConstants.begin(), randomIndex);
                    number->setValue(newValue);
                }
                else if (std::abs(value) > 0.001L && std::abs(value) < 100.0L) {
                    if (std::abs(value - 2.0L) < 0.5L) number->setValue(2.0L);  // Octave
                    else if (std::abs(value - 0.707L) < 0.2L) number->setValue(0.707L); // -3dB
                    else if (std::abs(value - 1.414L) < 0.2L) number->setValue(1.414L); // sqrt(2)
                    else if (std::abs(value - 3.14159L) < 0.5L) number->setValue(3.14159L); // π
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

class ExponentialPatternEnhancer final : public Changer {
public:
    ExponentialPatternEnhancer() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::EXPONENTIAL_PATTERN_ENHANCER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto value = number->calculate();
                
                if (std::abs(value) > 1.0L && std::abs(value) < 10.0L) {
                    // Create power-of-2 relationships
                    auto powers = {1.0L, 2.0L, 4.0L, 8.0L, 0.5L, 0.25L, 0.125L};
                    auto randomIndex = randomNumber_.calculate(0, static_cast<int>(powers.size()) - 1);
                    auto newValue = *std::next(powers.begin(), randomIndex);
                    number->setValue(newValue);
                }
                else if (std::abs(value) < 1.0L && std::abs(value) > 0.01L) {
                    auto fractions = {0.5L, 0.25L, 0.125L, 0.333L, 0.667L, 0.1L, 0.01L};
                    auto randomIndex = randomNumber_.calculate(0, static_cast<int>(fractions.size()) - 1);
                    auto newValue = *std::next(fractions.begin(), randomIndex);
                    number->setValue(newValue);
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

class PowerRelationshipPromoter final : public Changer {
public:
    PowerRelationshipPromoter() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::POWER_RELATIONSHIP_PROMOTER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto value = number->calculate();
                
                if (std::abs(value) < 10.0L) {
                    const auto powerExponents = {
                        0.5L,   // Square root relationship
                        1.0L,   // Linear relationship  
                        2.0L,   // Quadratic relationship
                        3.0L,   // Cubic relationship
                        0.333L, // Cube root relationship
                        0.25L,  // Fourth root relationship
                        1.5L,   // 3/2 power (common in physics)
                        -1.0L,  // Inverse relationship
                        -0.5L   // Inverse square root
                    };
                    
                    auto randomIndex = randomNumber_.calculate(0, static_cast<int>(powerExponents.size()) - 1);
                    auto newValue = *std::next(powerExponents.begin(), randomIndex);
                    number->setValue(newValue);
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

#endif
