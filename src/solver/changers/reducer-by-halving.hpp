#ifndef GENERIC_SOLVER_CHANGERS_REDUCER_BY_HALVING_HPP
#define GENERIC_SOLVER_CHANGERS_REDUCER_BY_HALVING_HPP

#include "changer-base.hpp"

class ReducerByHalving : public Changer {
public:
    ReducerByHalving() : Changer() {}

    template<typename C>
    explicit ReducerByHalving(C &coin) : Changer(coin) {}

    Formula change(Formula &formula) override {
        for (auto val: formula.getNumbers()) {
            if (coin->toss()) {
                val->setValue(val->calculate() / 2);
            }
        }
        return formula;
    }

    ChangerType getType() override {
        return ChangerType::REDUCER_BY_HALVING;
    }
};

#endif