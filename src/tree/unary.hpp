#ifndef GENERIC_SOLVER_TREE_UNARY_HPP
#define GENERIC_SOLVER_TREE_UNARY_HPP

#include "tree-base.hpp"
#include <array>
#include <cmath>

enum class UnaryOperationType {
    SIN,
    COS,
    TAN,
    SQUARE,
    CUBE,
    SQUARE_ROOT,
    SQUARE_ROOT_NEG,
    CUBE_ROOT,
    LOG,
    LOG_10,
    LOG_2,
    EXP
};

static constexpr std::array<UnaryOperationType, 12> UNARY_OPERATIONS{
    UnaryOperationType::SIN, UnaryOperationType::COS, UnaryOperationType::TAN,
    UnaryOperationType::SQUARE, UnaryOperationType::CUBE, UnaryOperationType::SQUARE_ROOT,
    UnaryOperationType::SQUARE_ROOT_NEG, UnaryOperationType::CUBE_ROOT, UnaryOperationType::LOG,
    UnaryOperationType::LOG_10, UnaryOperationType::LOG_2, UnaryOperationType::EXP
};

class UnaryOperation : public Node {
public:
    template<typename O>
    UnaryOperation(string symbol, const O& operand)
            : symbol_(std::move(symbol)), operand_(make_shared<O>(operand)) {}

    UnaryOperation(string symbol, NodePtr operand)
            : symbol_(std::move(symbol)), operand_(std::move(operand)) {}

    UnaryOperation(string symbol, number value)
            : symbol_(std::move(symbol)), operand_(make_shared<Number>(value)) {}

    string toString() const override {
        return symbol_ + '(' + (operand_ ? operand_->toString() : "null") + ')';
    }

    [[nodiscard]] virtual string toCppCode() const override {
        const string operandCode = operand_ ? operand_->toCppCode() : "0.0L";
        return getCppFunction() + "(" + operandCode + ")";
    }

    [[nodiscard]] number calculate() const override = 0;

    [[nodiscard]] NodePtr getOperand() const noexcept { return operand_; }

    void setOperand(NodePtr operand) noexcept {
        operand_ = std::move(operand);
    }

protected:
    virtual string getCppFunction() const { return symbol_; }
    const string symbol_;
    NodePtr operand_;
};

class Sine : public UnaryOperation {
public:
    template<typename O>
    explicit Sine(const O& operand) : UnaryOperation("sin", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::sin(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::sin"; }
};

class Cosine : public UnaryOperation {
public:
    template<typename O>
    explicit Cosine(const O& operand) : UnaryOperation("cos", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::cos(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::cos"; }
};

class Tangent : public UnaryOperation {
public:
    template<typename O>
    explicit Tangent(const O& operand) : UnaryOperation("tan", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::tan(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::tan"; }
};

class Square : public UnaryOperation {
public:
    template<typename O>
    explicit Square(const O& operand) : UnaryOperation("^2", operand) {}

    [[nodiscard]] number calculate() const override {
        const number value = operand_->calculate();
        return value * value;
    }

    [[nodiscard]] string toString() const override {
        return "(" + (operand_ ? operand_->toString() : "null") + ')' + symbol_;
    }

    [[nodiscard]] string toCppCode() const override {
        const string operandCode = operand_ ? operand_->toCppCode() : "0.0L";
        return "std::pow(" + operandCode + ", 2.0L)";
    }
};

class Cube : public UnaryOperation {
public:
    template<typename O>
    explicit Cube(const O& operand) : UnaryOperation("^3", operand) {}

    [[nodiscard]] number calculate() const override {
        const number value = operand_->calculate();
        return value * value * value;
    }

    [[nodiscard]] string toString() const override {
        return "(" + (operand_ ? operand_->toString() : "null") + ')' + symbol_;
    }

    [[nodiscard]] string toCppCode() const override {
        const string operandCode = operand_ ? operand_->toCppCode() : "0.0L";
        return "std::pow(" + operandCode + ", 3.0L)";
    }
};

class SquareRoot : public UnaryOperation {
public:
    template<typename O>
    explicit SquareRoot(const O& operand) : UnaryOperation("sqrt", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::sqrt(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::sqrt"; }
};

class SquareRootNegative : public SquareRoot {
public:
    template<typename O>
    explicit SquareRootNegative(const O& operand) : SquareRoot(operand) {}

    [[nodiscard]] number calculate() const override { 
        return -std::sqrt(operand_->calculate()); 
    }
};

class CubeRoot : public UnaryOperation {
public:
    template<typename O>
    explicit CubeRoot(const O& operand) : UnaryOperation("cbrt", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::cbrt(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::cbrt"; }
};

class LogarithmNatural : public UnaryOperation {
public:
    template<typename O>
    explicit LogarithmNatural(const O& operand) : UnaryOperation("log", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::log(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::log"; }
};

class LogarithmCommon : public UnaryOperation {
public:
    template<typename O>
    explicit LogarithmCommon(const O& operand)
            : UnaryOperation("log10", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::log10(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::log10"; }
};

class LogarithmBinary : public UnaryOperation {
public:
    template<typename O>
    explicit LogarithmBinary(const O& operand) : UnaryOperation("log2", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::log2(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::log2"; }
};

class Exponentiation : public UnaryOperation {
public:
    template<typename O>
    explicit Exponentiation(const O& operand) : UnaryOperation("exp", operand) {}

    [[nodiscard]] number calculate() const override { 
        return std::exp(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::exp"; }
};

#endif