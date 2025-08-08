#ifndef GENERIC_SOLVER_PRECISION_TUNER_HPP
#define GENERIC_SOLVER_PRECISION_TUNER_HPP

#include "changer-base.hpp"
#include "../../utils/base.hpp"

class PrecisionTuner final : public Changer {
public:
    PrecisionTuner() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::PRECISION_TUNER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                if (std::abs(currentValue) > 0.001L) {
                    auto adjustmentPercent = randomNumber_.calculate(-10, 10) * 0.001L;
                    auto newValue = currentValue * (1.0L + adjustmentPercent);
                    number->setValue(newValue);
                }
                else {
                    auto adjustment = randomNumber_.calculate(-100, 100) * 0.00001L;
                    auto newValue = currentValue + adjustment;
                    number->setValue(newValue);
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

class RangeOptimizer final : public Changer {
public:
    RangeOptimizer() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::RANGE_OPTIMIZER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                if (std::abs(currentValue) < 0.1L) {
                    const auto scalingFactors = {10.0L, 50.0L, 100.0L, 20.0L, 30.0L};
                    auto idx = randomNumber_.calculate(0, static_cast<int>(scalingFactors.size()) - 1);
                    auto scale = *std::next(scalingFactors.begin(), idx);
                    number->setValue(currentValue * scale);
                }
                else if (std::abs(currentValue) > 100.0L) {
                    const auto reductions = {0.1L, 0.5L, 0.2L, 0.3L, 0.01L};
                    auto idx = randomNumber_.calculate(0, static_cast<int>(reductions.size()) - 1);
                    auto reduction = *std::next(reductions.begin(), idx);
                    number->setValue(currentValue * reduction);
                }
                else {
                    auto fineAdjustment = randomNumber_.calculate(-50, 50) * 0.01L;
                    number->setValue(currentValue * (1.0L + fineAdjustment));
                }
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

class NonlinearityInjector final : public Changer {
public:
    NonlinearityInjector() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::NONLINEARITY_INJECTOR;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                // Apply nonlinear transformations to the values themselves
                auto idx = randomNumber_.calculate(0, 6); // 7 different transformations
                
                long double newValue = currentValue;
                switch (idx) {
                    case 0: newValue = std::tanh(currentValue); break;               // Bounded sigmoid-like
                    case 1: newValue = 1.0L / (1.0L + std::exp(-currentValue)); break; // Sigmoid
                    case 2: newValue = std::abs(currentValue); break;               // Rectification
                    case 3: newValue = currentValue * currentValue; break;         // Quadratic
                    case 4: newValue = std::exp(std::min(currentValue, 10.0L)); break; // Safe exponential
                    case 5: newValue = currentValue > 0 ? std::log(currentValue + 1.0L) : -std::log(-currentValue + 1.0L); break; // Safe log
                    case 6: newValue = currentValue / (1.0L + std::abs(currentValue)); break; // Soft saturation
                    default: break;
                }
                
                number->setValue(newValue);
                break;
            }
        }
        
        return result;
    }

private:
    mutable AlmostRandomNumber randomNumber_;
};

#endif
