#ifndef GENERIC_SOLVER_TREE_BINARY_HPP
#define GENERIC_SOLVER_TREE_BINARY_HPP

#include "tree-base.hpp"
#include <cmath>
#include <limits>
#include <array>

enum class BinaryOperationType {
    ADD, SUB, MUL, DIV, POW
};

static constexpr std::array<BinaryOperationType, 5> BINARY_OPERATIONS{
    BinaryOperationType::ADD, BinaryOperationType::SUB, BinaryOperationType::MUL, 
    BinaryOperationType::DIV, BinaryOperationType::POW
};

class BinaryOperation : public Node {
public:
    BinaryOperation(string symbol, NodePtr left, NodePtr right)
        : symbol_(std::move(symbol)), left_(std::move(left)), right_(std::move(right)) {}

    // Convenience overloads for numeric literals
    BinaryOperation(string symbol, number left, NodePtr right)
        : symbol_(std::move(symbol)), left_(std::make_shared<Number>(left)), right_(std::move(right)) {}
    BinaryOperation(string symbol, NodePtr left, number right)
        : symbol_(std::move(symbol)), left_(std::move(left)), right_(std::make_shared<Number>(right)) {}
    BinaryOperation(string symbol, number left, number right)
        : symbol_(std::move(symbol)), left_(std::make_shared<Number>(left)), right_(std::make_shared<Number>(right)) {}

    [[nodiscard]] string toString() const override {
        return "(" + left_->toString() + symbol_ + right_->toString() + ")";
    }

    [[nodiscard]] virtual string toCppCode() const override {
        const string leftCode = left_ ? left_->toCppCode() : "0.0L";
        const string rightCode = right_ ? right_->toCppCode() : "0.0L";
        return "(" + leftCode + " " + getCppOperator() + " " + rightCode + ")";
    }

    [[nodiscard]] NodePtr getLeft() const noexcept { return left_; }
    void setLeft(NodePtr node) noexcept { left_ = std::move(node); }

    [[nodiscard]] NodePtr getRight() const noexcept { return right_; }
    void setRight(NodePtr node) noexcept { right_ = std::move(node); }

    [[nodiscard]] number calculate() const override = 0;

protected:
    virtual string getCppOperator() const { return symbol_; }
    string symbol_;
    NodePtr left_;
    NodePtr right_;
};

class Addition : public BinaryOperation {
public:
    Addition(NodePtr left, NodePtr right)
        : BinaryOperation("+", std::move(left), std::move(right)) {}
    Addition(number left, NodePtr right)
        : BinaryOperation("+", left, std::move(right)) {}
    Addition(NodePtr left, number right)
        : BinaryOperation("+", std::move(left), right) {}
    Addition(number left, number right)
        : BinaryOperation("+", left, right) {}

    [[nodiscard]] number calculate() const override { 
        return left_->calculate() + right_->calculate(); 
    }

    [[nodiscard]] NodePtr simplify() const override {
        auto leftS = left_->simplify();
        auto rightS = right_->simplify();

        // 0 + x = x
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (std::abs(leftNum->calculate()) < EPSILON) return rightS;
        }
        // x + 0 = x
        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (std::abs(rightNum->calculate()) < EPSILON) return leftS;
        }
        // Constant folding: number + number = result
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
    Subtraction(NodePtr left, NodePtr right)
        : BinaryOperation("-", std::move(left), std::move(right)) {}
    Subtraction(number left, NodePtr right)
        : BinaryOperation("-", left, std::move(right)) {}
    Subtraction(NodePtr left, number right)
        : BinaryOperation("-", std::move(left), right) {}
    Subtraction(number left, number right)
        : BinaryOperation("-", left, right) {}

    [[nodiscard]] number calculate() const override { 
        return left_->calculate() - right_->calculate(); 
    }

    [[nodiscard]] NodePtr simplify() const override {
        auto leftS = left_->simplify();
        auto rightS = right_->simplify();

        // x - 0 = x
        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (std::abs(rightNum->calculate()) < EPSILON) return leftS;
        }
        // Constant folding
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
    Multiplication(NodePtr left, NodePtr right)
        : BinaryOperation("*", std::move(left), std::move(right)) {}
    Multiplication(number left, NodePtr right)
        : BinaryOperation("*", left, std::move(right)) {}
    Multiplication(NodePtr left, number right)
        : BinaryOperation("*", std::move(left), right) {}
    Multiplication(number left, number right)
        : BinaryOperation("*", left, right) {}

    [[nodiscard]] number calculate() const override { 
        return left_->calculate() * right_->calculate(); 
    }

    [[nodiscard]] NodePtr simplify() const override {
        auto leftS = left_->simplify();
        auto rightS = right_->simplify();

        // 0 * x = 0, x * 0 = 0
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (std::abs(leftNum->calculate()) < EPSILON) return leftS;
            if (std::abs(leftNum->calculate() - 1.0L) < EPSILON) return rightS;
        }
        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (std::abs(rightNum->calculate()) < EPSILON) return rightS;
            if (std::abs(rightNum->calculate() - 1.0L) < EPSILON) return leftS;
        }
        // Constant folding
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
    Division(NodePtr left, NodePtr right)
        : BinaryOperation("/", std::move(left), std::move(right)) {}
    Division(number left, NodePtr right)
        : BinaryOperation("/", left, std::move(right)) {}
    Division(NodePtr left, number right)
        : BinaryOperation("/", std::move(left), right) {}
    Division(number left, number right)
        : BinaryOperation("/", left, right) {}

    [[nodiscard]] number calculate() const override { 
        const auto divisor = right_->calculate();
        if (std::abs(divisor) < DIVISION_BY_ZERO_THRESHOLD) {
            return std::numeric_limits<number>::quiet_NaN();
        }
        return left_->calculate() / divisor; 
    }

    [[nodiscard]] NodePtr simplify() const override {
        auto leftS = left_->simplify();
        auto rightS = right_->simplify();

        // x / 1 = x
        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (std::abs(rightNum->calculate() - 1.0L) < EPSILON) return leftS;
        }
        // 0 / x = 0 (if x != 0)
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (std::abs(leftNum->calculate()) < EPSILON) return leftS;
        }
        // Constant folding
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
                const auto divisor = rightNum->calculate();
                if (std::abs(divisor) > DIVISION_BY_ZERO_THRESHOLD) {
                    return std::make_shared<Number>(leftNum->calculate() / divisor);
                }
            }
        }
        return std::make_shared<Division>(leftS, rightS);
    }
};

class Power : public BinaryOperation {
public:
    Power(NodePtr left, NodePtr right)
        : BinaryOperation("^", std::move(left), std::move(right)) {}
    Power(number left, NodePtr right)
        : BinaryOperation("^", left, std::move(right)) {}
    Power(NodePtr left, number right)
        : BinaryOperation("^", std::move(left), right) {}
    Power(number left, number right)
        : BinaryOperation("^", left, right) {}

    [[nodiscard]] number calculate() const override { 
        const auto base = left_->calculate();
        const auto exponent = right_->calculate();
        
        // Handle special cases
        if (std::isnan(base) || std::isnan(exponent)) {
            return std::numeric_limits<number>::quiet_NaN();
        }
        
        return std::pow(base, exponent); 
    }

    [[nodiscard]] string toCppCode() const override {
        const string leftCode = left_ ? left_->toCppCode() : "0.0L";
        const string rightCode = right_ ? right_->toCppCode() : "0.0L";
        return "std::pow(" + leftCode + ", " + rightCode + ")";
    }

    [[nodiscard]] NodePtr simplify() const override {
        auto leftS = left_->simplify();
        auto rightS = right_->simplify();

        // x^0 = 1, x^1 = x
        if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
            if (std::abs(rightNum->calculate()) < EPSILON) {
                return std::make_shared<Number>(1.0L);
            }
            if (std::abs(rightNum->calculate() - 1.0L) < EPSILON) {
                return leftS;
            }
        }
        // 0^x = 0 (for x > 0), 1^x = 1
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (std::abs(leftNum->calculate()) < EPSILON) {
                return leftS; // 0^x = 0
            }
            if (std::abs(leftNum->calculate() - 1.0L) < EPSILON) {
                return leftS; // 1^x = 1
            }
        }
        // Constant folding
        if (auto leftNum = std::dynamic_pointer_cast<Number>(leftS)) {
            if (auto rightNum = std::dynamic_pointer_cast<Number>(rightS)) {
                const auto result = std::pow(leftNum->calculate(), rightNum->calculate());
                if (std::isfinite(result)) {
                    return std::make_shared<Number>(result);
                }
            }
        }
        return std::make_shared<Power>(leftS, rightS);
    }
};
#endif