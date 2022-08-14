#ifndef GENERIC_SOLVER_TREE_BINARY_HPP
#define GENERIC_SOLVER_TREE_BINARY_HPP

#include "tree-base.hpp"

enum BinaryOperationType {
    ADD, SUB, MULT, DIV, POW, HYP
};
static vector<BinaryOperationType> BINARY_OPERATIONS({ADD, SUB, MULT, DIV, POW, HYP});

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

    shared_ptr<Node> getLeft() { return left; }

    void setLeft(shared_ptr<Node> node) { left = std::move(node); }

    shared_ptr<Node> getRight() { return right; }

    void setRight(shared_ptr<Node> node) { right = std::move(node); }

    number calculate() override = 0;

protected:
    string symbol;
    shared_ptr<Node> left;
    shared_ptr<Node> right;
};

class Addition : public BinaryOperation {
public:
    template<typename L, typename R>
    Addition(const L &left, const R &right) : BinaryOperation("+", left, right) {}

    number calculate() override { return left->calculate() + right->calculate(); }
};

class Subtraction : public BinaryOperation {
public:
    template<typename L, typename R>
    Subtraction(const L &left, const R &right)
            : BinaryOperation("-", left, right) {}

    number calculate() override { return left->calculate() - right->calculate(); }
};

class Multiplication : public BinaryOperation {
public:
    template<typename L, typename R>
    Multiplication(const L &left, const R &right)
            : BinaryOperation("*", left, right) {}

    number calculate() override { return left->calculate() * right->calculate(); }
};

class Division : public BinaryOperation {
public:
    template<typename L, typename R>
    Division(const L &left, const R &right) : BinaryOperation("/", left, right) {}

    number calculate() override { return left->calculate() / right->calculate(); }
};

class Power : public BinaryOperation {
public:
    template<typename L, typename R>
    Power(const L &left, const R &right) : BinaryOperation("^", left, right) {}

    number calculate() override {
        return pow(left->calculate(), right->calculate());
    }
};

class Hypotenuse : public BinaryOperation {
public:
    template<typename L, typename R>
    Hypotenuse(const L &left, const R &right)
            : BinaryOperation("hypot", left, right) {}

    number calculate() override {
        return hypot(left->calculate(), right->calculate());
    }
};

#endif
