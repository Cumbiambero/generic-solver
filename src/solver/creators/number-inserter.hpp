#ifndef GENERIC_SOLVER_CHANGERS_FLIPPER_HPP
#define GENERIC_SOLVER_CHANGERS_FLIPPER_HPP

#include "creator-base.hpp"

class NumberInserter : public Creator {
public:
    NumberInserter() : Creator() {}

    template<typename C, typename N>
    explicit NumberInserter(C &coin, N &randomNumber) : Creator(coin,randomNumber) {}

    Formula insert(Formula &formula) {
        auto one = shared_ptr<Node>(new Number(1));
        auto node = operationProducer->createBinaryOperation(formula.getRoot(), one);
        Formula result(node, formula.getVariables());
        return result;
    }
};

#endif