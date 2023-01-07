#ifndef GENERIC_SOLVER_CHANGERS_REDUCER_BY_ONE_HPP
#define GENERIC_SOLVER_CHANGERS_REDUCER_BY_ONE_HPP

#include "changer-base.hpp"

class ReducerByOne : public Changer {
public:
    ReducerByOne() : Changer() {}

    template<typename C>
    explicit ReducerByOne(C &coin) : Changer(coin) {}

    Formula change(Formula &formula) override {
        for (auto val: formula.getNumbers()) {
            if (coin->toss()) {
                val->setValue(val->calculate() - 1);
            }
        }
        return formula;
    }

    ChangerType getType() override {
        return ChangerType::REDUCER_BY_ONE;
    }
};

#endif