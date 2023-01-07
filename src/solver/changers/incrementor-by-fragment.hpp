#ifndef GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_FRAGMENT_HPP
#define GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_FRAGMENT_HPP

#include "changer-base.hpp"
#include <cmath>
#include <cfloat>

class IncrementorByFragment : public Changer {
public:
    IncrementorByFragment() : Changer() {}

    template<typename C>
    explicit IncrementorByFragment(C &coin) : Changer(coin) {}

    Formula change(Formula &formula) override {
        for (auto val: formula.getNumbers()) {
            number current = val->calculate();
            if (coin->toss()) {
                int exponent;
                number mantissa = frexp(current, &exponent);
                if (mantissa == 0) {
                    val->setValue(DBL_MIN);
                }
                mantissa += DBL_EPSILON;
                val->setValue(ldexp(mantissa, exponent));
            }
        }
        return formula;
    }

    ChangerType getType() override {
        return ChangerType::INCREMENTOR_BY_FRAGMENT;
    }
};

#endif