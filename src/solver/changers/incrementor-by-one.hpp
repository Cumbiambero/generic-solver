#ifndef GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_ONE_HPP
#define GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_ONE_HPP

#include "changer-base.hpp"

class IncrementorByOne : public Changer {
public:
    IncrementorByOne() : Changer() {}

    template<typename C>
    explicit IncrementorByOne(C &coin) : Changer(coin) {}

    Formula change(const Formula &formula) override {
        Formula result(formula);
        for (auto val: result.getNumbers()) {
            if (coin->toss()) {
                val->setValue(val->calculate() + 1);
            }
        }
        return result;
    }

    ChangerType getType() override {
        return INCREMENTOR_BY_ONE;
    }
};

#endif