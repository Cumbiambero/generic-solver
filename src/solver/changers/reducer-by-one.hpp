#ifndef GENERIC_SOLVER_CHANGERS_REDUCER_BY_ONE_HPP
#define GENERIC_SOLVER_CHANGERS_REDUCER_BY_ONE_HPP

#include "changer-base.hpp"

class ReducerByOne : public Changer {
public:
    ReducerByOne() : Changer() {}

    template<typename C>
    explicit ReducerByOne(C &coin) : Changer(coin) {}

    Formula change(const Formula &formula) override {
        Formula result(formula);
        for (auto val: result.getNumbers()) {
            if (coin->toss()) {
                val->setValue(val->calculate() - 1);
            }
        }
        return result;
    }

    ChangerType getType() override {
        return ChangerType::REDUCER_BY_ONE;
    }
};

#endif