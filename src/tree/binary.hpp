#ifndef GENERIC_SOLVER_TREE_BINARY_HPP
#define GENERIC_SOLVER_TREE_BINARY_HPP

#include "tree-base.hpp"

enum class BinaryOperationType {
    ADD, SUB, MUL, DIV, POW
};
static vector<BinaryOperationType> BINARY_OPERATIONS(
        {BinaryOperationType::ADD, BinaryOperationType::SUB, BinaryOperationType::MUL, BinaryOperationType::DIV,
         BinaryOperationType::POW});

class BinaryOperation : public Node {
public:
    BinaryOperation(string symbol, std::shared_ptr<Node> left, std::shared_ptr<Node> right)
            : symbol(std::move(symbol)), left(std::move(left)), right(std::move(right)) {}

    template<typename L, typename R>
    BinaryOperation(string symbol, const L &left, const R &right)
            : symbol(std::move(symbol)), left(make_shared<L>(left)),
              right(make_shared<R>(right)) {}

    template<typename R>
    BinaryOperation(string symbol, const number left, const R &right)
            : symbol(std::move(symbol)), left(make_shared<Number>(Number(left))),
              right(make_shared<R>(right)) {}

    template<typename L>
    BinaryOperation(string symbol, const L &left, const number right)
            : symbol(std::move(symbol)), left(make_shared<L>(left)),
              right(make_shared<Number>(Number(right))) {}

    BinaryOperation(string symbol, const number left, const number right)
            : symbol(std::move(symbol)), left(make_shared<Number>(Number(left))),
              right(make_shared<Number>(Number(right))) {}

    string toString() override {
        return "(" + left->toString() + symbol + right->toString() + ")";
    }

    shared_ptr<Node> &getLeft() { return left; }

    void setLeft(shared_ptr<Node> node) { left = std::move(node); }

    shared_ptr<Node> &getRight() { return right; }

    void setRight(shared_ptr<Node> node) { right = std::move(node); }

    number calculate() override = 0;

protected:
    string symbol;
    shared_ptr<Node> left;
    shared_ptr<Node> right;
};

class Addition : public BinaryOperation {
public:
    Addition(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
        : BinaryOperation("+", std::move(left), std::move(right)) {}

    number calculate() override { return left->calculate() + right->calculate(); }

    std::shared_ptr<Node> simplify() const override {
        auto leftS = left->simplify();
        auto rightS = right->simplify();

        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (leftNum->calculate() == 0.0) return rightS;
        }
        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (rightNum->calculate() == 0.0) return leftS;
        }
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
                return std::make_shared<Number>(leftNum->calculate() + rightNum->calculate());
            }
        }
        return std::make_shared<Addition>(leftS, rightS);
    }
};

class Subtraction : public BinaryOperation {
public:
    Subtraction(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
        : BinaryOperation("-", std::move(left), std::move(right)) {}

    number calculate() override { return left->calculate() - right->calculate(); }

    std::shared_ptr<Node> simplify() const override {
        auto leftS = left->simplify();
        auto rightS = right->simplify();

        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (rightNum->calculate() == 0.0) return leftS;
        }
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
                return std::make_shared<Number>(leftNum->calculate() - rightNum->calculate());
            }
        }
        return std::make_shared<Subtraction>(leftS, rightS);
    }
};

class Multiplication : public BinaryOperation {
public:
    Multiplication(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
        : BinaryOperation("*", std::move(left), std::move(right)) {}

    number calculate() override { return left->calculate() * right->calculate(); }

    std::shared_ptr<Node> simplify() const override {
        auto leftS = left->simplify();
        auto rightS = right->simplify();

        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (leftNum->calculate() == 0.0) return leftS;
            if (leftNum->calculate() == 1.0) return rightS;
        }
        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (rightNum->calculate() == 0.0) return rightS;
            if (rightNum->calculate() == 1.0) return leftS;
        }
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
                return std::make_shared<Number>(leftNum->calculate() * rightNum->calculate());
            }
        }
        return std::make_shared<Multiplication>(leftS, rightS);
    }
};

class Division : public BinaryOperation {
public:
    Division(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
        : BinaryOperation("/", std::move(left), std::move(right)) {}

    number calculate() override { return left->calculate() / right->calculate(); }

    std::shared_ptr<Node> simplify() const override {
        auto leftS = left->simplify();
        auto rightS = right->simplify();

        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (rightNum->calculate() == 1.0) return leftS;
        }
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (leftNum->calculate() == 0.0) return leftS;
        }
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
                return std::make_shared<Number>(leftNum->calculate() / rightNum->calculate());
            }
        }
        return std::make_shared<Division>(leftS, rightS);
    }
};

class Power : public BinaryOperation {
public:
    Power(std::shared_ptr<Node> left, std::shared_ptr<Node> right)
        : BinaryOperation("^", std::move(left), std::move(right)) {}

    number calculate() override { return std::pow(left->calculate(), right->calculate()); }

    std::shared_ptr<Node> simplify() const override {
        auto leftS = left->simplify();
        auto rightS = right->simplify();

        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (rightNum->calculate() == 0.0) return std::make_shared<Number>(1.0);
            if (rightNum->calculate() == 1.0) return leftS;
        }
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (leftNum->calculate() == 0.0) return std::make_shared<Number>(0.0);
        }
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
                return std::make_shared<Number>(std::pow(leftNum->calculate(), rightNum->calculate()));
            }
        }
        return std::make_shared<Power>(leftS, rightS);
    }
};
#endif