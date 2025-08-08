#ifndef GENERIC_SOLVER_CHANGERS_REDUCER_BY_FRAGMENT_HPP
#define GENERIC_SOLVER_CHANGERS_REDUCER_BY_FRAGMENT_HPP

#include "changer-base.hpp"
#include <cmath>
#include <limits>

class ReducerByFragment : public Changer {
public:
    ReducerByFragment() : Changer() {}

    template<typename C>
    explicit ReducerByFragment(C& coin) : Changer(coin) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        for (auto* val : result.getNumbers()) {
            const number current = val->calculate();
            if (coin_->toss()) {
                int exponent;
                number mantissa = std::frexp(current, &exponent);
                if (mantissa == 0.0L) {
                    val->setValue(std::numeric_limits<number>::min());
                } else {
                    mantissa -= std::numeric_limits<number>::epsilon();
                    val->setValue(std::ldexp(mantissa, exponent));
                }
            }
        }
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::REDUCER_BY_FRAGMENT;
    }
};

#endif