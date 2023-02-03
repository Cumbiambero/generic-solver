#ifndef GENERIC_SOLVER_TREE_UNARY_HPP
#define GENERIC_SOLVER_TREE_UNARY_HPP

#include "tree-base.hpp"

enum class UnaryOperationType {
    SIN,
    COS,
    ARC_SIN,
    ARC_COS,
    TAN,
    ARC_TAN,
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
static vector<UnaryOperationType> UNARY_OPERATIONS(
        {UnaryOperationType::SIN, UnaryOperationType::COS, UnaryOperationType::ARC_SIN, UnaryOperationType::ARC_COS,
         UnaryOperationType::TAN, UnaryOperationType::ARC_TAN, UnaryOperationType::SQUARE, UnaryOperationType::CUBE,
         UnaryOperationType::SQUARE_ROOT, UnaryOperationType::SQUARE_ROOT_NEG, UnaryOperationType::CUBE_ROOT,
         UnaryOperationType::LOG, UnaryOperationType::LOG_10,
         UnaryOperationType::LOG_2, UnaryOperationType::EXP});

class UnaryOperation : public Node {
public:
    template<typename O>
    UnaryOperation(string symbol, const O &operand)
            : symbol(std::move(symbol)), operand(make_shared<O>(operand)) {}

    UnaryOperation(string symbol, std::shared_ptr<Node> operand)
            : symbol(std::move(symbol)), operand(std::move(operand)) {}

    UnaryOperation(string symbol, number value)
            : symbol(std::move(symbol)), operand(make_shared<Number>(Number(value))) {}

    string toString() override {
        return symbol + '(' + operand->toString() + ')';
    };

    number calculate() override = 0;

    shared_ptr<Node> &getOperand() { return operand; }

protected:
    shared_ptr<Node> operand;
    const string symbol;
};

class Sine : public UnaryOperation {
public:
    template<typename O>
    explicit Sine(const O &operand) : UnaryOperation("sin", operand) {}

    number calculate() override { return sin(operand->calculate()); };
};

class ArcSine : public UnaryOperation {
public:
    template<typename O>
    explicit ArcSine(const O &operand) : UnaryOperation("asin", operand) {}

    number calculate() override { return asin(operand->calculate()); };
};

class Cosine : public UnaryOperation {
public:
    template<typename O>
    explicit Cosine(const O &operand) : UnaryOperation("cos", operand) {}

    number calculate() override { return cos(operand->calculate()); };
};

class ArcCosine : public UnaryOperation {
public:
    template<typename O>
    explicit ArcCosine(const O &operand) : UnaryOperation("acos", operand) {}

    number calculate() override { return acos(operand->calculate()); };
};

class Tangent : public UnaryOperation {
public:
    template<typename O>
    explicit Tangent(const O &operand) : UnaryOperation("tan", operand) {}

    number calculate() override { return tan(operand->calculate()); };
};

class ArcTangent : public UnaryOperation {
public:
    template<typename O>
    explicit ArcTangent(const O &operand) : UnaryOperation("atan", operand) {}

    number calculate() override { return atan(operand->calculate()); };
};

class Square : public UnaryOperation {
public:
    template<typename O>
    explicit Square(const O &operand) : UnaryOperation("²", operand) {}

    number calculate() override {
        number value = operand->calculate();
        return value * value;
    };

    string toString() override {
        return "(" + operand->toString() + ')' + symbol;
    };
};

class Cube : public UnaryOperation {
public:
    template<typename O>
    explicit Cube(const O &operand) : UnaryOperation("³", operand) {}

    number calculate() override {
        number value = operand->calculate();
        return value * value * value;
    };

    string toString() override {
        return "(" + operand->toString() + ')' + symbol;
    };
};

class SquareRoot : public UnaryOperation {
public:
    template<typename O>
    explicit SquareRoot(const O &operand) : UnaryOperation("√", operand) {}

    number calculate() override { return sqrt(operand->calculate()); };
};

class SquareRootNegative : public SquareRoot {
public:
    template<typename O>
    explicit SquareRootNegative(const O &operand) : SquareRoot(operand) {}

    number calculate() override { return -sqrt(operand->calculate()); };
};

class CubeRoot : public UnaryOperation {
public:
    template<typename O>
    explicit CubeRoot(const O &operand) : UnaryOperation("∛", operand) {}

    number calculate() override { return cbrt(operand->calculate()); };
};

class LogarithmNatural : public UnaryOperation {
public:
    template<typename O>
    explicit LogarithmNatural(const O &operand) : UnaryOperation("log", operand) {}

    number calculate() override { return log(operand->calculate()); };
};

class LogarithmCommon : public UnaryOperation {
public:
    template<typename O>
    explicit LogarithmCommon(const O &operand)
            : UnaryOperation("log10", operand) {}

    number calculate() override { return log10(operand->calculate()); };
};

class LogarithmBinary : public UnaryOperation {
public:
    template<typename O>
    explicit LogarithmBinary(const O &operand) : UnaryOperation("log2", operand) {}

    number calculate() override { return log(operand->calculate()); };
};

class Exponentiation : public UnaryOperation {
public:
    template<typename O>
    explicit Exponentiation(const O &operand) : UnaryOperation("exp", operand) {}

    number calculate() override { return exp(operand->calculate()); };
};

#endif