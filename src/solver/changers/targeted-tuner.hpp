#ifndef GENERIC_SOLVER_TARGETED_TUNER_HPP
#define GENERIC_SOLVER_TARGETED_TUNER_HPP

#include "changer-base.hpp"
#include "../../utils/base.hpp"

class TargetedTuner final : public Changer {
public:
    TargetedTuner() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::TARGETED_TUNER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                if (std::abs(currentValue - 1.0L) < 0.2L) {
                    auto adjustment = randomNumber_.calculate(-20, 20) * 0.001L;
                    number->setValue(currentValue + adjustment);
                }
                else if (std::abs(currentValue - 10.0L) < 2.0L) {
                    auto adjustment = randomNumber_.calculate(-50, 50) * 0.001L;
                    number->setValue(currentValue + adjustment);
                }
                else if (std::abs(currentValue) < 0.1L) {
                    auto adjustment = randomNumber_.calculate(-10, 10) * 0.0001L;
                    number->setValue(currentValue + adjustment);
                }
                else if (currentValue > 50.0L) {
                    auto reduction = randomNumber_.calculate(1, 10) * 0.001L;
                    number->setValue(currentValue * (1.0L - reduction));
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

class PatternOptimizer final : public Changer {
public:
    PatternOptimizer() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::PATTERN_OPTIMIZER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                if (std::abs(currentValue - 1.0L) < 0.5L) {
                    const auto successfulBases = {
                        1.0L, 0.932L, 1.091L, 1.095L, 1.046L, 0.5L, 1.5L
                    };
                    auto idx = randomNumber_.calculate(0, static_cast<int>(successfulBases.size()) - 1);
                    auto newValue = *std::next(successfulBases.begin(), idx);
                    number->setValue(newValue);
                }
                else if (std::abs(currentValue - 10.0L) < 3.0L) {
                    const auto successfulScales = {
                        10.0L, 11.0L, 9.955L, 10.085L, 9.921L
                    };
                    auto idx = randomNumber_.calculate(0, static_cast<int>(successfulScales.size()) - 1);
                    auto newValue = *std::next(successfulScales.begin(), idx);
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
