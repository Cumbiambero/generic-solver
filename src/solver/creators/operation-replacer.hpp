#ifndef GENERIC_SOLVER_CREATORS_OPERATION_REPLACER_HPP
#define GENERIC_SOLVER_CREATORS_OPERATION_REPLACER_HPP

#include "creator-base.hpp"

class OperationReplacer : public Creator {
public:
    OperationReplacer() : Creator() {}

    template<typename C, typename N>
    explicit OperationReplacer(C &coin, N &randomNumber) : Creator(coin, randomNumber) {}

    Formula change(Formula &formula) override {
        Formula result(formula);
        traverse(result.getRoot());
        return result;
    }

    ChangerType getType() override {
        return ChangerType::OPERATION_REPLACER;
    }

private:
    void traverse(shared_ptr<Node> node) {
        if (node == nullptr) {
            return;
        }
        auto *wrapper = dynamic_cast<Wrapper *>(node.get());
        if (wrapper != nullptr) {
            traverse(wrapper->getNode());
        }
        auto *binary = dynamic_cast<BinaryOperation *>(node.get());
        if (binary == nullptr) {
            auto *unary = dynamic_cast<UnaryOperation *>(node.get());
            if (unary != nullptr && coin->toss()) {
                auto created = operationProducer->createUnaryOperation(unary->getOperand());
                node.swap(created);
                if (coin->toss()) {
                    return;
                }
                traverse(unary->getOperand());
            }
        } else {
            if (coin->toss()) {
                auto created = operationProducer->createBinaryOperation(binary->getLeft(), binary->getRight());
                node.swap(created);
                if (coin->toss()) {
                    return;
                }
            }
            traverse(binary->getLeft());
            traverse(binary->getRight());
        }
    }
};

#endif