#ifndef GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_DOUBLING_HPP
#define GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_DOUBLING_HPP

#include "changer-base.hpp"

class IncrementorByDoubling : public Changer {
public:
    IncrementorByDoubling() : Changer() {}

    template<typename C>
    explicit IncrementorByDoubling(C& coin) : Changer(coin) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula; // Make a copy
        for (auto* val : result.getNumbers()) {
            if (coin_->toss()) {
                val->setValue(val->calculate() * 2.0L);
            }
        }
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::INCREMENTOR_BY_DOUBLING;
    }
};

#endif