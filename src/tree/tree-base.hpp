#ifndef GENERIC_SOLVER_TREE_BASE_HPP
#define GENERIC_SOLVER_TREE_BASE_HPP

#include "../utils/base.hpp"
#include "../utils/config.hpp"
#include <cmath>
#include <iomanip>
#include <memory>
#include <utility>
#include <string_view>

// Forward declarations
class Node;
using NodePtr = std::shared_ptr<Node>;
using ConstNodePtr = std::shared_ptr<const Node>;

class Node : public std::enable_shared_from_this<Node> {
public:
    virtual ~Node() = default;
    
    [[nodiscard]] virtual string toString() const = 0;
    [[nodiscard]] virtual string toCppCode() const = 0;  // New method for C++ code generation
    [[nodiscard]] virtual number calculate() const = 0;
    [[nodiscard]] virtual NodePtr simplify() const {
        return std::const_pointer_cast<Node>(shared_from_this());
    }
    
    // Prevent copying of base class
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    
    // Allow moving
    Node(Node&&) = default;
    Node& operator=(Node&&) = default;

protected:
    Node() = default;
};

class Wrapper : public Node {
public:
    Wrapper() = delete;

    explicit Wrapper(NodePtr node) : node_(std::move(node)) {}

    template<typename N>
    explicit Wrapper(const N& nodeObj) : node_(std::make_shared<N>(nodeObj)) {}

    [[nodiscard]] string toString() const override { 
        return node_ ? node_->toString() : "null"; 
    }

    [[nodiscard]] string toCppCode() const override {
        return node_ ? node_->toCppCode() : "0.0L";
    }

    [[nodiscard]] number calculate() const override { 
        return node_ ? node_->calculate() : 0.0L; 
    }

    [[nodiscard]] NodePtr getNode() const noexcept { return node_; }

    void setNode(NodePtr node) noexcept {
        node_ = std::move(node);
    }

    [[nodiscard]] NodePtr simplify() const override {
        return node_ ? std::make_shared<Wrapper>(node_->simplify()) : nullptr;
    }

private:
    NodePtr node_;
};

class Number : public Node {
public:
    explicit Number(number value) noexcept : value_(value) {}

    [[nodiscard]] string toString() const override {
        std::ostringstream oss;
        oss << std::setprecision(FORMULA_NUMBER_PRECISION) << std::noshowpoint << value_;
        return value_ > 0 ? oss.str() : "(" + oss.str() + ")";
    }

    [[nodiscard]] string toCppCode() const override {
        std::ostringstream oss;
        oss << std::setprecision(FORMULA_NUMBER_PRECISION) << std::noshowpoint << value_ << "L";
        return oss.str();
    }

    [[nodiscard]] number calculate() const override { return value_; }

    virtual void setValue(number val) noexcept {
        value_ = val;
    }

    [[nodiscard]] NodePtr simplify() const override {
        return std::static_pointer_cast<Number>(
            const_cast<Number*>(this)->shared_from_this());
    }

protected:
    number value_;
};

class Variable : public Number {
public:
    explicit Variable(string symbol)
            : Variable(std::move(symbol), 1.0L) {}

    Variable(string symbol, number value) noexcept
            : Number(value), symbol_(std::move(symbol)) {}

    // Explicit copy constructor - Variable needs to be copyable for vector storage
    Variable(const Variable& other) 
            : Number(other.value_), symbol_(other.symbol_) {}

    // Explicit copy assignment
    Variable& operator=(const Variable& other) {
        if (this != &other) {
            value_ = other.value_;
            symbol_ = other.symbol_;
        }
        return *this;
    }

    // Default move semantics
    Variable(Variable&&) = default;
    Variable& operator=(Variable&&) = default;

    [[nodiscard]] string toString() const override { return symbol_; }

    [[nodiscard]] string toCppCode() const override { return symbol_; }

    [[nodiscard]] NodePtr simplify() const override {
        return std::static_pointer_cast<Variable>(
            const_cast<Variable*>(this)->shared_from_this());
    }

private:
    string symbol_;  // Made non-const to allow move semantics
};

class Constant : public Number {
public:
    Constant(string symbol, number value) noexcept
            : Number(value), symbol_(std::move(symbol)) {}

    [[nodiscard]] string toString() const override { return symbol_; }

    [[nodiscard]] string toCppCode() const override {
        if (symbol_ == "π") return "M_PI";
        if (symbol_ == "e") return "M_E";
        return Number::toCppCode(); // Fall back to numeric value
    }

    void setValue([[maybe_unused]] number val) noexcept override { 
        // Constants cannot be changed
    }

    [[nodiscard]] NodePtr simplify() const override {
        return std::static_pointer_cast<Constant>(
            const_cast<Constant*>(this)->shared_from_this());
    }

private:
    const string symbol_;
};

class Pi : public Constant {
public:
    Pi() : Constant("π", 3.141592653589793238462643383279502884L) {}
};

class Euler : public Constant {
public:
    Euler() : Constant("e", 2.718281828459045235360287471352662498L) {}
};

#endif