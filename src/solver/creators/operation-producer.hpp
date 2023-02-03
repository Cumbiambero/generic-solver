#ifndef GENERIC_SOLVER_CREATORS_OPERATION_PRODUCER_HPP
#define GENERIC_SOLVER_CREATORS_OPERATION_PRODUCER_HPP

#include "../solver-base.hpp"

class OperationProducer {
public:
    OperationProducer() : randomNumber(make_shared<AlmostRandomNumber>()) {}

    template<typename N>
    explicit OperationProducer(N &randomNumber) : randomNumber(make_shared<N>(randomNumber)) {}

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

    template<typename O>
    shared_ptr<Node> createUnaryOperation(const O& operand) {
        switch (pickRandomUnaryOperationType()) {
            case SIN:
                return make_shared<Sine>(Sine(operand));
            case COS:
                return make_shared<Cosine>(Cosine(operand));
            case ARC_SIN:
                return make_shared<ArcSine>(ArcSine(operand));
            case ARC_COS:
                return make_shared<ArcCosine>(ArcCosine(operand));
            case TAN:
                return make_shared<Tangent>(Tangent(operand));
            case ARC_TAN:
                return make_shared<ArcTangent>(ArcTangent(operand));
            case SQUARE:
                return make_shared<Square>(Square(operand));
            case CUBE:
                return make_shared<Cube>(Cube(operand));
            case SQUARE_ROOT:
                return make_shared<SquareRoot>(SquareRoot(operand));
            case SQUARE_ROOT_NEG:
                return make_shared<SquareRootNegative>(SquareRootNegative(operand));
            case CUBE_ROOT:
                return make_shared<CubeRoot>(CubeRoot(operand));
            case LOG:
                return make_shared<LogarithmNatural>(LogarithmNatural(operand));
            case LOG_10:
                return make_shared<LogarithmCommon>(LogarithmCommon(operand));
            case LOG_2:
                return make_shared<LogarithmBinary>(LogarithmBinary(operand));
            case EXP:
                return make_shared<Exponentiation>(Exponentiation(operand));
        }
        return nullptr;
    }

    template<typename L, typename R>
    shared_ptr<Node> createBinaryOperation(const L& left, const R& right) {
        switch (pickRandomBinaryOperationType()) {
            case ADD:
                return make_shared<Addition>(Addition(left, right));
            case SUB:
                return make_shared<Subtraction>(Subtraction(left, right));
            case MUL:
                return make_shared<Multiplication>(Multiplication(left, right));
            case DIV:
                return make_shared<Division>(Division(left, right));
            case POW:
                return make_shared<Power>(Power(left, right));
            case HYP:
                return make_shared<Hypotenuse>(Hypotenuse(left, right));
        }
        return nullptr;
    }

private:
    shared_ptr<RandomNumber> randomNumber;

    UnaryOperationType pickRandomUnaryOperationType() {
        return UNARY_OPERATIONS[randomNumber->calculate(0, UNARY_OPERATIONS.size() - 1)];
    }

    BinaryOperationType pickRandomBinaryOperationType() {
        return BINARY_OPERATIONS[randomNumber->calculate(0, BINARY_OPERATIONS.size() - 1)];
    }

    shared_ptr<Node> createUnaryOperation(vector<Variable> params) {
        decltype((params[0])) operand = params[0];
        return createUnaryOperation(operand);
    }

    shared_ptr<Node> createBinaryOperation(vector<Variable> params) {
        decltype((params[0])) left = params[0];
        decltype((params[1])) right = params[1];
        return createBinaryOperation(left, right);
    }

    shared_ptr<Node> createMultipleBinaryOperations(const vector<Variable> &params, size_t size) {
        vector<shared_ptr<Node>> operators;
        for (int i = 0; i < size - 1; i += 2) {
            operators.push_back(createBinaryOperation((vector<Variable>) {params[i], params[i + 1]}));
        }
        if (size % 2 == 1) {
            operators.push_back(make_shared<Variable>(params[size - 1]));
        }

        shared_ptr result = operators.back();
        operators.pop_back();

        while (!operators.empty()) {
            result = createBinaryOperation(operators.back(), result);
            operators.pop_back();
        }
        return result;
    }
};

#endif