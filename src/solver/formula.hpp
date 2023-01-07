#ifndef GENERIC_SOLVER_FORMULA_HPP
#define GENERIC_SOLVER_FORMULA_HPP

#include "../tree/tree.hpp"
#include <map>
#include <mutex>
#include <set>

#include <iostream>
#include <utility>

class Formula {
public:
    template<typename N>
    Formula(N &node, Variable params...) : Formula(node, vector<Variable>{std::move(params)}) {}

    template<typename N>
    Formula(N &node, const vector<Variable> variables)
            : variables(variables), root(make_shared<Wrapper>(Wrapper(node))) {
        init(variables);
    }

    Formula(const Formula &copy)
            : variables(copy.variables),
              root(createCopy(copy.root)) {
        for (size_t i = 0; i < variables.size(); i++) {
            Variable variable = variables[i];
            variablePositions[i] = variableNames[variable.toString()];
        }
    }

    virtual ~Formula() = default;

    number evaluate(number params...) {
        std::vector<number> values = {params};
        size_t numberOfParams = values.size();
        if (variableNames.size() != numberOfParams) {
            throw invalid_argument("the number of params does not match with the number of params");
        }

        lock.lock();
        if (!variablePositions.empty() && variablePositions.size() == values.size()) {
            for (size_t i = 0; i < numberOfParams; i++) {
                Variable *&variable = variablePositions[i];
                if (variable == nullptr) {
                    lock.unlock();
                    return 0;
                }
                variable->setValue(values[i]);
            }
        }
        number result = root->calculate();
        lock.unlock();

        return result;
    }

    [[nodiscard]] string toString() const { return root->toString(); }

    [[nodiscard]] shared_ptr<Node> &getRoot() { return root; }

    vector<BinaryOperation *> &getBinaryOperators() { return binaryOperators; }

    [[nodiscard]] vector<Variable> getVariables() const {
        return variables;
    }

    vector<Number *> getNumbers() {
        return this->numbers;
    }

    bool operator==(const Formula &rhs) const {
        return root->toString() == rhs.root->toString();
    }

    bool operator!=(const Formula &rhs) const {
        return !(rhs == *this);
    }

private:
    mutex lock;
    set<Constant *> constants;
    vector<BinaryOperation *> binaryOperators;
    vector<Number *> numbers;
    map<size_t, Variable *> variablePositions;
    map<string, Variable *> variableNames;
    vector<Variable> variables;
    shared_ptr<Node> root;

    void init(const vector<Variable> &vars) {
        variableNames.clear();
        variablePositions.clear();
        traverse(root);
        for (size_t i = 0; i < vars.size(); i++) {
            Variable variable = vars[i];
            variablePositions[i] = variableNames[variable.toString()];
        }
    }

    void traverse(const shared_ptr<Node> &current) {
        auto *binary = dynamic_cast<BinaryOperation *>(current.get());
        if (binary == nullptr) {
            auto *unary = dynamic_cast<UnaryOperation *>(current.get());
            if (unary != nullptr) {
                traverse(unary->getOperand());
            }
            auto *constant = dynamic_cast<Constant *>(current.get());
            if (constant != nullptr) {
                constants.insert(constant);
            }
            auto *variable = dynamic_cast<Variable *>(current.get());
            if (variable != nullptr) {
                variableNames[variable->toString()] = variable;
            }
            auto *number = dynamic_cast<Number *>(current.get());
            if (number != nullptr && variable == nullptr) {
                numbers.push_back(number);
            }
            auto *wrapper = dynamic_cast<Wrapper *>(current.get());
            if (wrapper != nullptr) {
                traverse(wrapper->getNode());
            }
            return;
        }

        traverse(binary->getLeft());
        traverse(binary->getRight());
        binaryOperators.push_back(binary);
    }

    shared_ptr<Node> createCopy(shared_ptr<Node> original) {
        auto *binary = dynamic_cast<BinaryOperation *>(original.get());
        if (binary == nullptr) {
            auto *unary = dynamic_cast<UnaryOperation *>(original.get());
            if (unary != nullptr) {
                auto *cosine = dynamic_cast<Cosine *>(original.get());
                if (cosine != nullptr) {
                    return make_shared<Cosine>(createCopy(cosine->getOperand()));
                }
                auto *cube = dynamic_cast<Cube *>(original.get());
                if (cube != nullptr) {
                    return make_shared<Cube>(createCopy(cube->getOperand()));
                }
                auto *cubeRoot = dynamic_cast<CubeRoot *>(original.get());
                if (cubeRoot != nullptr) {
                    return make_shared<CubeRoot>(createCopy(cubeRoot->getOperand()));
                }
                auto *exponentiation = dynamic_cast<Exponentiation *>(original.get());
                if (exponentiation != nullptr) {
                    return make_shared<Exponentiation>(createCopy(exponentiation->getOperand()));
                }
                auto *logarithmBinary = dynamic_cast<LogarithmBinary *>(original.get());
                if (logarithmBinary != nullptr) {
                    return make_shared<LogarithmBinary>(createCopy(logarithmBinary->getOperand()));
                }
                auto *logarithmCommon = dynamic_cast<LogarithmCommon *>(original.get());
                if (logarithmCommon != nullptr) {
                    return make_shared<LogarithmCommon>(createCopy(logarithmCommon->getOperand()));
                }
                auto *logarithmNatural = dynamic_cast<LogarithmNatural *>(original.get());
                if (logarithmNatural != nullptr) {
                    return make_shared<LogarithmNatural>(createCopy(logarithmNatural->getOperand()));
                }
                auto *sine = dynamic_cast<Sine *>(original.get());
                if (sine != nullptr) {
                    return make_shared<Sine>(createCopy(sine->getOperand()));
                }
                auto *square = dynamic_cast<Square *>(original.get());
                if (square != nullptr) {
                    return make_shared<Square>(createCopy(square->getOperand()));
                }
                auto *squareRoot = dynamic_cast<SquareRoot *>(original.get());
                if (squareRoot != nullptr) {
                    return make_shared<SquareRoot>(createCopy(squareRoot->getOperand()));
                }
                auto *squareRootNegative = dynamic_cast<SquareRootNegative *>(original.get());
                if (squareRootNegative != nullptr) {
                    return make_shared<SquareRootNegative>(createCopy(squareRootNegative->getOperand()));
                }
                auto *tangent = dynamic_cast<Tangent *>(original.get());
                if (tangent != nullptr) {
                    return make_shared<Tangent>(createCopy(tangent->getOperand()));
                }
            }
            auto *constant = dynamic_cast<Constant *>(original.get());
            if (constant != nullptr) {
                auto copy = shared_ptr(original);
                constants.insert(constant);
                return copy;
            }
            auto *variable = dynamic_cast<Variable *>(original.get());
            if (variable != nullptr) {
                auto variableName = variable->toString();
                auto variableCopy = make_shared<Variable>(variableName);
                variableNames[variableName] = variableCopy.get();
                return variableCopy;
            }
            auto *number = dynamic_cast<Number *>(original.get());
            if (number != nullptr) {
                auto copy = make_shared<Number>(number->calculate());
                numbers.push_back(copy.get());
                return copy;
            }
            auto *wrapper = dynamic_cast<Wrapper *>(original.get());
            if (wrapper != nullptr) {
                return make_shared<Wrapper>(createCopy(wrapper->getNode()));
            }
        }

        auto *addition = dynamic_cast<Addition *>(original.get());
        if (addition != nullptr) {
            auto copy = make_shared<Addition>(createCopy(addition->getLeft()), createCopy(addition->getRight()));
            binaryOperators.push_back(copy.get());
            return copy;
        }
        auto *division = dynamic_cast<Division *>(original.get());
        if (division != nullptr) {
            auto copy = make_shared<Division>(createCopy(division->getLeft()), createCopy(division->getRight()));
            binaryOperators.push_back(copy.get());
            return copy;
        }
        auto *mult = dynamic_cast<Multiplication *>(original.get());
        if (mult != nullptr) {
            auto copy = make_shared<Multiplication>(createCopy(mult->getLeft()), createCopy(mult->getRight()));
            binaryOperators.push_back(copy.get());
            return copy;
        }
        auto *power = dynamic_cast<Power *>(original.get());
        if (power != nullptr) {
            auto copy = make_shared<Power>(createCopy(power->getLeft()), createCopy(power->getRight()));
            binaryOperators.push_back(copy.get());
            return copy;
        }
        auto *sub = dynamic_cast<Subtraction *>(original.get());
        if (sub != nullptr) {
            auto copy = make_shared<Subtraction>(createCopy(sub->getLeft()), createCopy(sub->getRight()));
            binaryOperators.push_back(copy.get());
            return copy;
        }
        return original;
    }
};

#endif