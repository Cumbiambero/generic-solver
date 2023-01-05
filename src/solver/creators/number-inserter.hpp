#ifndef GENERIC_SOLVER_CHANGERS_NUMBER_INSERTER_HPP
#define GENERIC_SOLVER_CHANGERS_NUMBER_INSERTER_HPP

#include "creator-base.hpp"

class NumberInserter : public Creator {
public:
    NumberInserter() : Creator() {}

    template<typename C, typename N>
    explicit NumberInserter(C &coin, N &randomNumber) : Creator(coin, randomNumber) {}

    Formula change(const Formula &formula) override {
        auto node = operationProducer->createBinaryOperation(formula.getRoot(), createNumberOrConstant());
        Formula result(node, formula.getVariables());
        return result;
    }

    ChangerType getType() override {
        return ChangerType::NUMBER_INSERTER;
    }

private:
    shared_ptr<Node> pi = shared_ptr<Node>(new Pi());
    shared_ptr<Node> e = shared_ptr<Node>(new Euler());

    [[nodiscard]] shared_ptr<Node> createNumberOrConstant() const {
        if (operationProducer->getRandomNumber()->calculate(0, 10) < 8) {
            return shared_ptr<Node>(new Number(1));
        } else {
            if (coin->toss()) {
                return pi;
            }
            return e;
        }
    }
};

#endif