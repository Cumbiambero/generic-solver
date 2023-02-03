#ifndef GENERIC_SOLVER_CHANGERS_FLIPPER_HPP
#define GENERIC_SOLVER_CHANGERS_FLIPPER_HPP

#include "changer-base.hpp"

class Flipper : public Changer {
public:
    Flipper() : Changer() {}

    template<typename C>
    explicit Flipper(C &coin) : Changer(coin) {}

    Formula change(const Formula &formula) override {
        Formula result(formula);
        for (auto binary: result.getBinaryOperators()) {
            if (coin->toss()) {
                auto temp = binary->getRight();
                binary->setRight(binary->getLeft());
                binary->setLeft(temp);
            }
        }
        return result;
    }

    ChangerType getType() override {
        return FLIPPER;
    }
};

#endif