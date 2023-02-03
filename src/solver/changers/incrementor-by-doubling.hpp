#ifndef GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_DOUBLING_HPP
#define GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_DOUBLING_HPP

#include "changer-base.hpp"

class IncrementorByDoubling : public Changer {
public:
    IncrementorByDoubling() : Changer() {}

    template<typename C>
    explicit IncrementorByDoubling(C &coin) : Changer(coin) {}

    Formula change(const Formula &formula) override {
        Formula result(formula);
        for (auto val: result.getNumbers()) {
            if (coin->toss()) {
                val->setValue(val->calculate() * 2);
            }
        }
        return result;
    }

    ChangerType getType() override {
        return INCREMENTOR_BY_DOUBLING;
    }
};

#endif