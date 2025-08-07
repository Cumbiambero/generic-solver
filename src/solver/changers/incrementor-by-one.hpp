#ifndef GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_ONE_HPP
#define GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_ONE_HPP

#include "changer-base.hpp"

class IncrementorByOne : public Changer {
public:
    IncrementorByOne() : Changer() {}

    template<typename C>
    explicit IncrementorByOne(C &coin) : Changer(coin) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula; // Make a copy
        for (auto* val : result.getNumbers()) {
            if (coin_->toss()) {
                val->setValue(val->calculate() + 1.0L);
            }
        }
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::INCREMENTOR_BY_ONE;
    }
};

#endif