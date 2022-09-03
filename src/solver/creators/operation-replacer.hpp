#ifndef GENERIC_SOLVER_CREATORS_OPERATION_REPLACER_HPP
#define GENERIC_SOLVER_CREATORS_OPERATION_REPLACER_HPP

#include "creator-base.hpp"

class OperationReplacer : public Creator {
public:
    OperationReplacer() : Creator() {}

    template<typename C, typename N>
    explicit OperationReplacer(C &coin, N &randomNumber) : Creator(coin, randomNumber) {}

    void replace(Formula &formula) {
        traverse(formula.getRoot());
    }

private:
    void traverse(shared_ptr<Node>& node) {
        auto *wrapper = dynamic_cast<Wrapper *>(node.get());
        if (wrapper != nullptr) {
            traverse(wrapper->getNode());
        }
        auto *binary = dynamic_cast<BinaryOperation *>(node.get());
        if (binary == nullptr) {
            auto *unary = dynamic_cast<UnaryOperation *>(node.get());
            if (unary != nullptr && coin->toss()) {
                traverse(unary->getOperand());
                auto created = operationProducer->createUnaryOperation(unary->getOperand());
                node.swap(created);
            }
        } else {
            if (coin->toss()) {
                auto created = operationProducer->createBinaryOperation(binary->getLeft(), binary->getRight());
                node.swap(created);
            }
            traverse(binary->getLeft());
            traverse(binary->getRight());
        }
    }
};

#endif