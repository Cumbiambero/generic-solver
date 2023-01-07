#ifndef GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_ONE_HPP
#define GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_ONE_HPP

#include "changer-base.hpp"

class IncrementorByOne : public Changer {
public:
    IncrementorByOne() : Changer() {}

    template<typename C>
    explicit IncrementorByOne(C &coin) : Changer(coin) {}

    Formula change(Formula &formula) override {
        for (auto val: formula.getNumbers()) {
            if (coin->toss()) {
                val->setValue(val->calculate() + 1);
            }
        }
        return formula;
    }

    ChangerType getType() override {
        return ChangerType::INCREMENTOR_BY_ONE;
    }
};

#endif