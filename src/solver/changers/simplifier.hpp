#ifndef GENERIC_SOLVER_SIMPLIFIER_HPP
#define GENERIC_SOLVER_SIMPLIFIER_HPP

#include "changer-base.hpp"

class Simplifier final : public Changer {
public:
    Simplifier() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::SIMPLIFIER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        if (numbers.size() > 3) {
            for (auto* number : numbers) {
                if (coin_->toss() && std::abs(number->calculate()) > 1e6L) {
                    number->setValue(std::round(number->calculate() / 1000L));
                }
            }
        }
        
        if (result.toString().length() > 100) {
            auto binaryOps = result.getBinaryOperators();
            if (binaryOps.size() > 5) {
                for ([[maybe_unused]] auto* binOp : binaryOps) {
                    if (coin_->toss()) {
                        if (coin_->toss()) {
                            break;
                        }
                    }
                }
            }
        }
        
        return result;
    }
};

#endif
