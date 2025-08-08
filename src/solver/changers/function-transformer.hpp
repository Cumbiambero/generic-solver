#ifndef GENERIC_SOLVER_FUNCTION_TRANSFORMER_HPP
#define GENERIC_SOLVER_FUNCTION_TRANSFORMER_HPP

#include "changer-base.hpp"

class FunctionTransformer final : public Changer {
public:
    FunctionTransformer() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::FUNCTION_TRANSFORMER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        for (auto* number : numbers) {
            if (coin_->toss()) {
                auto value = number->calculate();
                
                if (std::abs(value) > 1e6L) {
                    const auto simpleValues = {1.0L, 2.0L, 3.0L, 0.5L, 0.1L, -1.0L};
                    auto randomIndex = static_cast<int>(coin_->toss() ? 0 : simpleValues.size() - 1);
                    number->setValue(*std::next(simpleValues.begin(), randomIndex % static_cast<int>(simpleValues.size())));
                }
                else if (std::abs(value) > 0.001L && std::abs(value) < 100.0L) {
                    if (std::abs(value - 1.0L) < 0.5L) number->setValue(1.0L);
                    else if (std::abs(value - 2.0L) < 1.0L) number->setValue(2.0L);
                    else if (std::abs(value) < 1.0L) number->setValue(0.5L);
                }
            }
        }
        
        return result;
    }
};

#endif
