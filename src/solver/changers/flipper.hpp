#ifndef GENERIC_SOLVER_CHANGERS_FLIPPER_HPP
#define GENERIC_SOLVER_CHANGERS_FLIPPER_HPP

#include "changer-base.hpp"

class Flipper : public Changer {
public:
    Flipper() : Changer() {}

    template<typename C>
    explicit Flipper(C &coin) : Changer(coin) {}

    Formula change(Formula &formula) override {
        for (auto binary: formula.getBinaryOperators()) {
            if (coin->toss()) {
                auto temp = binary->getRight();
                binary->setRight(binary->getLeft());
                binary->setLeft(temp);
            }
        }
        return formula;
    }

    ChangerType getType() override {
        return ChangerType::FLIPPER;
    }
};

#endif