#ifndef GENERIC_SOLVER_CHANGERS_REDUCER_BY_HALVING_HPP
#define GENERIC_SOLVER_CHANGERS_REDUCER_BY_HALVING_HPP

#include "changer-base.hpp"

class ReducerByHalving : public Changer {
public:
    ReducerByHalving() : Changer() {}

    template<typename C>
    explicit ReducerByHalving(C &coin) : Changer(coin) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula; // Make a copy
        for (auto* val : result.getNumbers()) {
            if (coin_->toss()) {
                val->setValue(val->calculate() / 2.0L);
            }
        }
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::REDUCER_BY_HALVING;
    }
};

#endif