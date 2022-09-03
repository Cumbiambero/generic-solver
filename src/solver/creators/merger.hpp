#ifndef GENERIC_SOLVER_CREATORS_MERGER_HPP
#define GENERIC_SOLVER_CREATORS_MERGER_HPP

#include "creator-base.hpp"

class Merger : public Creator {
public:
    Merger() : Creator() {}

    template<typename C, typename N>
    explicit Merger(C &coin, N &randomNumber) : Creator(coin, randomNumber) {}

    Formula merge(Formula &formula1, Formula &formula2) {
        auto leftNode = traverse(formula1.getRoot());
        auto rightNode = traverse(formula2.getRoot());
        auto node = operationProducer->createBinaryOperation(leftNode, rightNode);
        Formula merged(node, formula1.getVariables());
        return merged;
    }

private:
    shared_ptr <Node> traverse(shared_ptr <Node> node) {
        auto binary = dynamic_cast<BinaryOperation *>(node.get());
        if (binary == nullptr) {
            auto unary = dynamic_cast<UnaryOperation *>(node.get());
            if (unary != nullptr) {
                if (coin->toss()) {
                    return unary->getOperand();
                }
                return traverse(unary->getOperand());
            }

            auto wrapper = dynamic_cast<Wrapper *>(node.get());
            if (wrapper != nullptr) {
                return traverse(wrapper->getNode());
            }
            return node;
        } else {
            if (coin->toss()) {
                return binary->getLeft();
            }
            traverse(binary->getLeft());

            if (coin->toss()) {
                return binary->getRight();
            }
            return traverse(binary->getRight());
        }
    }
};

#endif