#ifndef GENERIC_SOLVER_CHANGERS_DECREMENTOR_BY_FRAGMENT_HPP
#define GENERIC_SOLVER_CHANGERS_DECREMENTOR_BY_FRAGMENT_HPP

#include "changer-base.hpp"
#include <cmath>
#include <cfloat>

class DecrementorByFragment : public Changer {
public:
    DecrementorByFragment() : Changer() {}

    template<typename C>
    explicit DecrementorByFragment(C &coin) : Changer(coin) {}

    void change(Formula &formula) override {
        for (auto val: formula.getNumbers()) {
            number current = val->calculate();
            if (coin->toss()) {
                int exponent;
                number mantissa = frexp(current, &exponent);
                if (mantissa == 0) {
                    val->setValue(DBL_MIN);
                }
                mantissa -= DBL_EPSILON;
                val->setValue(ldexp(mantissa, exponent));
            }
        }
    }
};

#endif