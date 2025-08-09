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
    EXP,
    TANH,
    SINH,
    COSH,
    ASIN,
    ACOS,
    ATAN,
    ABS,
    FLOOR,
    CEIL,
    SIGMOID,
    SOFT_SAT
};

static constexpr std::array<UnaryOperationType, 23> UNARY_OPERATIONS{
    UnaryOperationType::SIN, UnaryOperationType::COS, UnaryOperationType::TAN,
    UnaryOperationType::SQUARE, UnaryOperationType::CUBE, UnaryOperationType::SQUARE_ROOT,
    UnaryOperationType::SQUARE_ROOT_NEG, UnaryOperationType::CUBE_ROOT, UnaryOperationType::LOG,
    UnaryOperationType::LOG_10, UnaryOperationType::LOG_2, UnaryOperationType::EXP,
    UnaryOperationType::TANH, UnaryOperationType::SINH, UnaryOperationType::COSH,
    UnaryOperationType::ASIN, UnaryOperationType::ACOS, UnaryOperationType::ATAN,
    UnaryOperationType::ABS, UnaryOperationType::FLOOR, UnaryOperationType::CEIL,
    UnaryOperationType::SIGMOID, UnaryOperationType::SOFT_SAT
};

class UnaryOperation : public Node {
public:
    UnaryOperation(string symbol, NodePtr operand)
        : symbol_(std::move(symbol)), operand_(std::move(operand)) {}

    explicit UnaryOperation(string symbol, number value)
        : symbol_(std::move(symbol)), operand_(std::make_shared<Number>(value)) {}

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
    explicit Sine(NodePtr operand) : UnaryOperation("sin", std::move(operand)) {}
    explicit Sine(number value) : UnaryOperation("sin", value) {}

    [[nodiscard]] number calculate() const override { 
        return std::sin(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::sin"; }
};

class Cosine : public UnaryOperation {
public:
    explicit Cosine(NodePtr operand) : UnaryOperation("cos", std::move(operand)) {}
    explicit Cosine(number value) : UnaryOperation("cos", value) {}

    [[nodiscard]] number calculate() const override { 
        return std::cos(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::cos"; }
};

class Tangent : public UnaryOperation {
public:
    explicit Tangent(NodePtr operand) : UnaryOperation("tan", std::move(operand)) {}
    explicit Tangent(number value) : UnaryOperation("tan", value) {}

    [[nodiscard]] number calculate() const override { 
        return std::tan(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::tan"; }
};

class Square : public UnaryOperation {
public:
    explicit Square(NodePtr operand) : UnaryOperation("^2", std::move(operand)) {}
    explicit Square(number value) : UnaryOperation("^2", value) {}

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
    explicit Cube(NodePtr operand) : UnaryOperation("^3", std::move(operand)) {}
    explicit Cube(number value) : UnaryOperation("^3", value) {}

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
    explicit SquareRoot(NodePtr operand) : UnaryOperation("sqrt", std::move(operand)) {}
    explicit SquareRoot(number value) : UnaryOperation("sqrt", value) {}

    [[nodiscard]] number calculate() const override { 
        return std::sqrt(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::sqrt"; }
};

class SquareRootNegative : public SquareRoot {
public:
    explicit SquareRootNegative(NodePtr operand) : SquareRoot(std::move(operand)) {}
    explicit SquareRootNegative(number value) : SquareRoot(value) {}

    [[nodiscard]] number calculate() const override { 
        return -std::sqrt(operand_->calculate()); 
    }
};

class CubeRoot : public UnaryOperation {
public:
    explicit CubeRoot(NodePtr operand) : UnaryOperation("cbrt", std::move(operand)) {}
    explicit CubeRoot(number value) : UnaryOperation("cbrt", value) {}

    [[nodiscard]] number calculate() const override { 
        return std::cbrt(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::cbrt"; }
};

class LogarithmNatural : public UnaryOperation {
public:
    explicit LogarithmNatural(NodePtr operand) : UnaryOperation("log", std::move(operand)) {}
    explicit LogarithmNatural(number value) : UnaryOperation("log", value) {}

    [[nodiscard]] number calculate() const override { 
        return std::log(operand_->calculate()); 
    }

protected:
    string getCppFunction() const override { return "std::log"; }
};

class LogarithmCommon : public UnaryOperation {
public:
    explicit LogarithmCommon(NodePtr operand)
        : UnaryOperation("log10", std::move(operand)) {}
    explicit LogarithmCommon(number value)
        : UnaryOperation("log10", value) {}

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

// New Enhanced Mathematical Functions
class HyperbolicTangent : public UnaryOperation {
public:
    template<typename O>
    explicit HyperbolicTangent(const O& operand) : UnaryOperation("tanh", operand) {}

    [[nodiscard]] number calculate() const override {
        return std::tanh(operand_->calculate());
    }

protected:
    string getCppFunction() const override { return "std::tanh"; }
};

class HyperbolicSine : public UnaryOperation {
public:
    template<typename O>
    explicit HyperbolicSine(const O& operand) : UnaryOperation("sinh", operand) {}

    [[nodiscard]] number calculate() const override {
        return std::sinh(operand_->calculate());
    }

protected:
    string getCppFunction() const override { return "std::sinh"; }
};

class HyperbolicCosine : public UnaryOperation {
public:
    template<typename O>
    explicit HyperbolicCosine(const O& operand) : UnaryOperation("cosh", operand) {}

    [[nodiscard]] number calculate() const override {
        return std::cosh(operand_->calculate());
    }

protected:
    string getCppFunction() const override { return "std::cosh"; }
};

class ArcSine : public UnaryOperation {
public:
    template<typename O>
    explicit ArcSine(const O& operand) : UnaryOperation("asin", operand) {}

    [[nodiscard]] number calculate() const override {
        const auto val = operand_->calculate();
        if (val < -1.0L || val > 1.0L) return 0.0L; // Handle domain error
        return std::asin(val);
    }

protected:
    string getCppFunction() const override { return "std::asin"; }
};

class ArcCosine : public UnaryOperation {
public:
    template<typename O>
    explicit ArcCosine(const O& operand) : UnaryOperation("acos", operand) {}

    [[nodiscard]] number calculate() const override {
        const auto val = operand_->calculate();
        if (val < -1.0L || val > 1.0L) return 0.0L; // Handle domain error
        return std::acos(val);
    }

protected:
    string getCppFunction() const override { return "std::acos"; }
};

class ArcTangent : public UnaryOperation {
public:
    template<typename O>
    explicit ArcTangent(const O& operand) : UnaryOperation("atan", operand) {}

    [[nodiscard]] number calculate() const override {
        return std::atan(operand_->calculate());
    }

protected:
    string getCppFunction() const override { return "std::atan"; }
};

class AbsoluteValue : public UnaryOperation {
public:
    template<typename O>
    explicit AbsoluteValue(const O& operand) : UnaryOperation("abs", operand) {}

    [[nodiscard]] number calculate() const override {
        return std::abs(operand_->calculate());
    }

protected:
    string getCppFunction() const override { return "std::abs"; }
};

class Floor : public UnaryOperation {
public:
    template<typename O>
    explicit Floor(const O& operand) : UnaryOperation("floor", operand) {}

    [[nodiscard]] number calculate() const override {
        return std::floor(operand_->calculate());
    }

protected:
    string getCppFunction() const override { return "std::floor"; }
};

class Ceiling : public UnaryOperation {
public:
    template<typename O>
    explicit Ceiling(const O& operand) : UnaryOperation("ceil", operand) {}

    [[nodiscard]] number calculate() const override {
        return std::ceil(operand_->calculate());
    }

protected:
    string getCppFunction() const override { return "std::ceil"; }
};

class Sigmoid : public UnaryOperation {
public:
    template<typename O>
    explicit Sigmoid(const O& operand) : UnaryOperation("sigmoid", operand) {}

    [[nodiscard]] number calculate() const override {
        const auto x = operand_->calculate();
        if (x > 500.0L) return 1.0L; // Prevent overflow
        if (x < -500.0L) return 0.0L;
        return 1.0L / (1.0L + std::exp(-x));
    }

protected:
    string getCppFunction() const override { 
        return "([](long double x) { return x > 500.0L ? 1.0L : (x < -500.0L ? 0.0L : 1.0L / (1.0L + std::exp(-x))); })";
    }
};

class SoftSaturation : public UnaryOperation {
public:
    template<typename O>
    explicit SoftSaturation(const O& operand) : UnaryOperation("sat", operand) {}

    [[nodiscard]] number calculate() const override {
        const auto x = operand_->calculate();
        return x / (1.0L + std::abs(x));
    }

protected:
    string getCppFunction() const override { 
        return "([](long double x) { return x / (1.0L + std::abs(x)); })";
    }
};

#endif