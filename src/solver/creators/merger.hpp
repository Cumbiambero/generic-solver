#ifndef GENERIC_SOLVER_CREATORS_MERGER_HPP
#define GENERIC_SOLVER_CREATORS_MERGER_HPP

#include "creator-base.hpp"

class Merger {
public:
    Merger() : coin_(make_shared<RandomCoin>()), operationProducer(make_shared<OperationProducer>()) {}

    template<typename C, typename R>
    explicit Merger(C &coin, R &randomNumber) : coin_(make_shared<C>(coin)), operationProducer(
            make_shared<OperationProducer>(OperationProducer(randomNumber))) {}

    Formula merge(const Formula& formula1, const Formula& formula2) {
        auto leftNode = traverse(formula1.getRoot());
        auto rightNode = traverse(formula2.getRoot());
        auto node = operationProducer->createBinaryOperation(leftNode, rightNode);
        Formula merged(node, formula1.getVariables());
        return merged;
    }

    [[nodiscard]] const shared_ptr<Coin> &getCoin() const {
        return coin_;
    }

private:
    shared_ptr<Coin> coin_;
    shared_ptr<OperationProducer> operationProducer;

    shared_ptr<Node> traverse(shared_ptr<Node> node) {
        auto binary = dynamic_cast<BinaryOperation *>(node.get());
        if (binary == nullptr) {
            auto unary = dynamic_cast<UnaryOperation *>(node.get());
            if (unary != nullptr) {
                if (coin_->toss()) {
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
            if (coin_->toss()) {
                return binary->getLeft();
            }
            traverse(binary->getLeft());

            if (coin_->toss()) {
                return binary->getRight();
            }
            return traverse(binary->getRight());
        }
    }
};

#endif