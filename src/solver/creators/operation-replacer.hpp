#ifndef GENERIC_SOLVER_CREATORS_OPERATION_REPLACER_HPP
#define GENERIC_SOLVER_CREATORS_OPERATION_REPLACER_HPP

#include "creator-base.hpp"

class OperationReplacer : public Creator {
public:
    OperationReplacer() : Creator() {}

    template<typename C, typename N>
    explicit OperationReplacer(C &coin, N &randomNumber) : Creator(coin, randomNumber) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result(formula);
        traverse(result.getRoot());
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::OPERATION_REPLACER;
    }

private:
    void traverse(shared_ptr<Node> node) const {
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
            if (unary != nullptr && coin_->toss()) {
                auto created = operationProducer->createUnaryOperation(unary->getOperand());
                node.swap(created);
                if (coin_->toss()) {
                    return;
                }
                traverse(unary->getOperand());
            }
        } else {
            if (coin_->toss()) {
                auto created = operationProducer->createBinaryOperation(binary->getLeft(), binary->getRight());
                node.swap(created);
                if (coin_->toss()) {
                    return;
                }
            }
            traverse(binary->getLeft());
            traverse(binary->getRight());
        }
    }
};

#endif