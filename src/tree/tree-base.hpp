#ifndef GENERIC_SOLVER_TREE_BASE_HPP
#define GENERIC_SOLVER_TREE_BASE_HPP

#include "../utils/base.hpp"
#include <cmath>
#include <iomanip>
#include <memory>
#include <utility>

class Node {
public:
    virtual ~Node() = default;

    virtual string toString() = 0;

    virtual number calculate() = 0;
};

class Wrapper : public Node {
public:
    Wrapper() = delete;
    explicit Wrapper(shared_ptr<Node> node) : node(std::move(node)) {}

    template<typename N>
    explicit Wrapper(const N &node) : node(make_shared<N>(node)) {}

    string toString() override { return node->toString(); }

    number calculate() override { return node->calculate(); }

    shared_ptr<Node>& getNode() { return node; }

protected:
    shared_ptr<Node> node;
};

class Value : public Node {
    string toString() override = 0;

    number calculate() override = 0;
};

class Number : public Value {
public:
    explicit Number(const number value) : value(value) {}

    string toString() override {
        ostringstream oss;
        oss << setprecision(8) << noshowpoint << value;
        return value > 0 ? oss.str() : "(" + oss.str() + ")";
    }

    number calculate() override { return value; };

    void setValue(number val) {
        this->value = val;
    }

protected:
    number value;
};

class Variable : public Number {
public:
    explicit Variable(string symbol)
            : Variable(std::move(symbol), 1) {}

    Variable(string symbol, const number value)
            : Number(value), symbol(std::move(symbol)) {}

    string toString() override { return symbol; }

private:
    const string symbol;
};

class Constant : public Number {
public:
    Constant(string symbol, const number value)
            : Number(value), symbol(std::move(symbol)) {}

    string toString() override { return symbol; }

private:
    const string symbol;
};

class Pi : public Constant {
public:
    Pi() : Constant("π", 3.141592653589793238462643383279502884L) {}
};

static Pi PI;

class Euler : public Constant {
public:
    Euler() : Constant("e", 2.718281828459045235360287471352662498L) {}
};

static const Euler E;

#endif