#ifndef GENERIC_SOLVER_CHANGERS_DECREMENTOR_BY_ONE_HPP
#define GENERIC_SOLVER_CHANGERS_DECREMENTOR_BY_ONE_HPP

#include "changer-base.hpp"

class DecrementorByOne : public Changer {
public:
    DecrementorByOne() : Changer() {}

    template<typename C>
    explicit DecrementorByOne(C &coin) : Changer(coin) {}

    void change(Formula &formula) override {
        for (auto val: formula.getNumbers()) {
            if (coin->toss()) {
                val->setValue(val->calculate() - 1);
            }
        }
    }
};

#endif