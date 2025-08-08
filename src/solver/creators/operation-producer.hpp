#ifndef GENERIC_SOLVER_CREATORS_OPERATION_PRODUCER_HPP
#define GENERIC_SOLVER_CREATORS_OPERATION_PRODUCER_HPP

#include "../../utils/arbitrary.hpp"
#include "../formula.hpp"

class OperationProducer {
public:
    OperationProducer() : randomNumber(make_shared<AlmostRandomNumber>()) {}

    shared_ptr<Node> produce(const vector<Variable> &params) {
        size_t size = params.size();
        if (size == 1) {
            return createUnaryOperation(params);
        } else if (size == 2) {
            return createBinaryOperation(params);
        } else if (size > 2) {
            return createMultipleBinaryOperations(params, size);
        }
        return nullptr;
    }

    shared_ptr<Node> createUnaryOperation(const shared_ptr<Node>& operand) {
        switch (pickRandomUnaryOperationType()) {
            case UnaryOperationType::SIN:
                return std::make_shared<Sine>(operand);
            case UnaryOperationType::COS:
                return std::make_shared<Cosine>(operand);
            case UnaryOperationType::TAN:
                return std::make_shared<Tangent>(operand);
            case UnaryOperationType::SQUARE:
                return std::make_shared<Square>(operand);
            case UnaryOperationType::CUBE:
                return std::make_shared<Cube>(operand);
            case UnaryOperationType::SQUARE_ROOT:
                return std::make_shared<SquareRoot>(operand);
            case UnaryOperationType::SQUARE_ROOT_NEG:
                return std::make_shared<SquareRootNegative>(operand);
            case UnaryOperationType::CUBE_ROOT:
                return std::make_shared<CubeRoot>(operand);
            case UnaryOperationType::LOG:
                return std::make_shared<LogarithmNatural>(operand);
            case UnaryOperationType::LOG_10:
                return std::make_shared<LogarithmCommon>(operand);
            case UnaryOperationType::LOG_2:
                return std::make_shared<LogarithmBinary>(operand);
            case UnaryOperationType::EXP:
                return std::make_shared<Exponentiation>(operand);
            case UnaryOperationType::TANH:
                return std::make_shared<HyperbolicTangent>(operand);
            case UnaryOperationType::SINH:
                return std::make_shared<HyperbolicSine>(operand);
            case UnaryOperationType::COSH:
                return std::make_shared<HyperbolicCosine>(operand);
            case UnaryOperationType::ASIN:
                return std::make_shared<ArcSine>(operand);
            case UnaryOperationType::ACOS:
                return std::make_shared<ArcCosine>(operand);
            case UnaryOperationType::ATAN:
                return std::make_shared<ArcTangent>(operand);
            case UnaryOperationType::ABS:
                return std::make_shared<AbsoluteValue>(operand);
            case UnaryOperationType::FLOOR:
                return std::make_shared<Floor>(operand);
            case UnaryOperationType::CEIL:
                return std::make_shared<Ceiling>(operand);
            case UnaryOperationType::SIGMOID:
                return std::make_shared<Sigmoid>(operand);
            case UnaryOperationType::SOFT_SAT:
                return std::make_shared<SoftSaturation>(operand);
            default:
                return nullptr;
        }
    }

    shared_ptr<Node> createBinaryOperation(const shared_ptr<Node>& left, const shared_ptr<Node>& right) {
        switch (pickRandomBinaryOperationType()) {
            case BinaryOperationType::ADD:
                return std::make_shared<Addition>(left, right);
            case BinaryOperationType::SUB:
                return std::make_shared<Subtraction>(left, right);
            case BinaryOperationType::MUL:
                return std::make_shared<Multiplication>(left, right);
            case BinaryOperationType::DIV:
                return std::make_shared<Division>(left, right);
            case BinaryOperationType::POW:
                return std::make_shared<Power>(left, right);
            default: 
                return nullptr;
        }
    }

    [[nodiscard]] const shared_ptr<RandomNumber> &getRandomNumber() const {
        return randomNumber;
    }

private:
    shared_ptr<RandomNumber> randomNumber;

    UnaryOperationType pickRandomUnaryOperationType() {
        const auto maxIndex = static_cast<int>(UNARY_OPERATIONS.size()) - 1;
        return UNARY_OPERATIONS[static_cast<size_t>(randomNumber->calculate(0, maxIndex))];
    }

    BinaryOperationType pickRandomBinaryOperationType() {
        const auto maxIndex = static_cast<int>(BINARY_OPERATIONS.size()) - 1;
        return BINARY_OPERATIONS[static_cast<size_t>(randomNumber->calculate(0, maxIndex))];
    }

    shared_ptr<Node> createUnaryOperation(vector<Variable> params) {
        auto operandPtr = std::make_shared<Variable>(params[0]);
        return createUnaryOperation(operandPtr);
    }

    shared_ptr<Node> createBinaryOperation(vector<Variable> params) {
        auto leftPtr = std::make_shared<Variable>(params[0]);
        auto rightPtr = std::make_shared<Variable>(params[1]);
        return createBinaryOperation(leftPtr, rightPtr);
    }

    shared_ptr<Node> createMultipleBinaryOperations(const vector<Variable> &params, size_t size) {
        vector<shared_ptr<Node>> operators;
        for (size_t i = 0; i < size - 1; i += 2) {
            auto leftPtr = std::make_shared<Variable>(params[i]);
            auto rightPtr = std::make_shared<Variable>(params[i + 1]);
            operators.push_back(createBinaryOperation(leftPtr, rightPtr));
        }
        if (size % 2 == 1) {
            operators.push_back(std::make_shared<Variable>(params[size - 1]));
        }

        shared_ptr<Node> result = operators.back();
        operators.pop_back();

        while (!operators.empty()) {
            result = createBinaryOperation(operators.back(), result);
            operators.pop_back();
        }
        return result;
    }
};

#endif