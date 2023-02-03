#ifndef GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_DOUBLING_HPP
#define GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_DOUBLING_HPP

#include "changer-base.hpp"

class IncrementorByDoubling : public Changer {
public:
    IncrementorByDoubling() : Changer() {}

    template<typename C>
    explicit IncrementorByDoubling(C &coin) : Changer(coin) {}

    Formula change(Formula &formula) override {
        for (auto val: formula.getNumbers()) {
            if (coin->toss()) {
                val->setValue(val->calculate() * 2);
            }
        }
        return formula;
    }

    ChangerType getType() override {
        return ChangerType::INCREMENTOR_BY_DOUBLING;
    }
};

#endif