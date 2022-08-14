#ifndef GENERIC_SOLVER_CHANGERS_FLIPPER_HPP
#define GENERIC_SOLVER_CHANGERS_FLIPPER_HPP

#include "../utils/arbitrary.hpp"
#include "../solver/formula.hpp"
#include <cmath>
#include <cstdlib>
#include <vector>

class Flipper {
public:
    Flipper() : coin(make_shared<RandomCoin>()) {}

    template<typename C>
    explicit Flipper(C &coin) : coin(make_shared<C>(coin)) {}

    void flip(Formula &formula) {
        for (auto binary: formula.getBinaryOperators()) {
            if (coin->toss()) {
                auto temp = binary->getRight();
                binary->setRight(binary->getLeft());
                binary->setLeft(temp);
            }
        }
    }

private:
    shared_ptr<Coin> coin;
};

#endif