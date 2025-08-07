#ifndef GENERIC_SOLVER_CHANGERS_FLIPPER_HPP
#define GENERIC_SOLVER_CHANGERS_FLIPPER_HPP

#include "changer-base.hpp"

class Flipper : public Changer {
public:
    Flipper() : Changer() {}

    template<typename C>
    explicit Flipper(C& coin) : Changer(coin) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula; // Make a copy
        for (auto* binary : result.getBinaryOperators()) {
            if (coin_->toss()) {
                auto temp = binary->getRight();
                binary->setRight(binary->getLeft());
                binary->setLeft(temp);
            }
        }
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::FLIPPER;
    }
};

#endif