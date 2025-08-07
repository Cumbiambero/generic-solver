#ifndef GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_FRAGMENT_HPP
#define GENERIC_SOLVER_CHANGERS_INCREMENTOR_BY_FRAGMENT_HPP

#include "changer-base.hpp"
#include <cmath>
#include <limits>

class IncrementorByFragment : public Changer {
public:
    IncrementorByFragment() : Changer() {}

    template<typename C>
    explicit IncrementorByFragment(C &coin) : Changer(coin) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula; // Make a copy
        for (auto* val : result.getNumbers()) {
            const number current = val->calculate();
            if (coin_->toss()) {
                int exponent;
                number mantissa = std::frexp(current, &exponent);
                if (mantissa == 0.0L) {
                    val->setValue(std::numeric_limits<number>::min());
                } else {
                    mantissa += std::numeric_limits<number>::epsilon();
                    val->setValue(std::ldexp(mantissa, exponent));
                }
            }
        }
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::INCREMENTOR_BY_FRAGMENT;
    }
};

#endif