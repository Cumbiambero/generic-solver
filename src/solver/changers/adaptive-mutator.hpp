#ifndef GENERIC_SOLVER_ADAPTIVE_MUTATOR_HPP

#include "../../utils/base.hpp"
#include "changer-base.hpp"
#include "../../utils/arbitrary.hpp"

class AdaptiveMutator final : public Changer {
public:
    AdaptiveMutator() : Changer(), 
                       stagnationCounter_(0), 
                       lastBestFitness_(0.0L),
                       mutationIntensity_(0.1L) {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::ADAPTIVE_MUTATOR;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        updateMutationIntensity();
        
        auto numbers = result.getNumbers();
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto currentValue = number->calculate();
                
                if (mutationIntensity_ > 0.5L) {
                    applyDramaticMutation(number, currentValue);
                } else if (mutationIntensity_ > 0.3L) {
                    applyModerateMutation(number, currentValue);
                } else {
                    applyFineTuningMutation(number, currentValue);
                }
            }
        }
        
        if (coin_->toss()) {
            auto binaryOps = result.getBinaryOperators();
            for (auto* binOp : binaryOps) {
                if (coin_->toss()) {
                    auto temp = binOp->getLeft();
                    binOp->setLeft(binOp->getRight());
                    binOp->setRight(temp);
                }
            }
        }
        
        return result;
    }
    
    void updateProgress(number currentBestFitness) const {
        if (std::abs(currentBestFitness - lastBestFitness_) < 1e-8L) {
            stagnationCounter_++;
        } else {
            stagnationCounter_ = 0;
            lastBestFitness_ = currentBestFitness;
        }
    }

private:
    mutable std::size_t stagnationCounter_;
    mutable number lastBestFitness_;
    mutable number mutationIntensity_;

    void updateMutationIntensity() const {
        if (stagnationCounter_ > 50) {
            mutationIntensity_ = std::min(1.0L, mutationIntensity_ + 0.1L);
        } else if (stagnationCounter_ == 0) {
            mutationIntensity_ = std::max(0.05L, mutationIntensity_ - 0.05L);
        }
        
        const number coolingFactor = 0.999L;
        mutationIntensity_ = std::max(0.05L, mutationIntensity_ * coolingFactor);
    }
    
    void applyDramaticMutation(Number* number, long double currentValue) const {
        const auto dramaticChanges = {
            0.0L, 1.0L, 2.0L, 0.5L, -1.0L, -2.0L, 
            10.0L, -10.0L, 0.1L, -0.1L
        };
        
        if (coin_->toss() && std::abs(currentValue) > 0.001L) {
            const auto scales = {10.0L, -10.0L, 0.1L, -0.1L, 100.0L, -100.0L};
            auto scaleIndex = randomNumber_.calculate(0, static_cast<int>(scales.size()) - 1);
            auto scale = *std::next(scales.begin(), scaleIndex);
            number->setValue(currentValue * scale);
        } else {
            auto randomIndex = randomNumber_.calculate(0, static_cast<int>(dramaticChanges.size()) - 1);
            auto newValue = *std::next(dramaticChanges.begin(), randomIndex);
            number->setValue(newValue);
        }
    }
    
    void applyModerateMutation(Number* number, long double currentValue) const {
        if (coin_->toss()) {
            const auto scales = {0.1L, 0.5L, 2.0L, 5.0L, 10.0L};
            auto randomIndex = randomNumber_.calculate(0, 4);
            auto scale = *std::next(scales.begin(), randomIndex);
            number->setValue(currentValue * scale);
        } else {            
            auto delta = randomNumber_.calculate(-10, 10) * 0.1L;
            number->setValue(currentValue + delta);
        }
    }
    
    void applyFineTuningMutation(Number* number, long double currentValue) const {
        auto delta = randomNumber_.calculate(-100, 100) * 0.001L;
        number->setValue(currentValue + delta);
    }
    
    mutable AlmostRandomNumber randomNumber_;
};

#endif
