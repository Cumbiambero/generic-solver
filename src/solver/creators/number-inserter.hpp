#ifndef GENERIC_SOLVER_CHANGERS_NUMBER_INSERTER_HPP
#define GENERIC_SOLVER_CHANGERS_NUMBER_INSERTER_HPP

#include "creator-base.hpp"

class NumberInserter : public Creator {
public:
    NumberInserter() : Creator() {}

    template<typename C, typename N>
    explicit NumberInserter(C &coin, N &randomNumber) : Creator(coin, randomNumber) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        auto node = operationProducer->createBinaryOperation(formula.getRoot(), createNumberOrConstant());
        Formula result(node, formula.getVariables());
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::NUMBER_INSERTER;
    }

private:
    shared_ptr<Node> pi = shared_ptr<Node>(new Pi());
    shared_ptr<Node> e = shared_ptr<Node>(new Euler());

    [[nodiscard]] shared_ptr<Node> createNumberOrConstant() const {
        int r = operationProducer->getRandomNumber()->calculate(0, 20);
        if (r < 12) {
            int val = operationProducer->getRandomNumber()->calculate(-5, 10);
            return std::make_shared<Number>(static_cast<number>(val));
        } else if (r < 16) { 
            number val = operationProducer->getRandomNumber()->calculate(1, 10) / 10.0; 
            return std::make_shared<Number>(val);
        } else if (r == 19) {
            return pi;
        } else {
            return e;
        }
    }
};

#endif