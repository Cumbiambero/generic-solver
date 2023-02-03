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

    [[nodiscard]] shared_ptr <Node> &getRoot() { return root; }

    [[nodiscard]] shared_ptr <Node> getRoot() const { return root; }

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
    shared_ptr <Node> root;

    void init(const vector<Variable> &vars) {
        variableNames.clear();
        variablePositions.clear();
        traverse(root);
        for (size_t i = 0; i < vars.size(); i++) {
            Variable variable = vars[i];
            variablePositions[i] = variableNames[variable.toString()];
        }
    }

    void traverse(const shared_ptr <Node> &current) {
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
        binaryOperators.push_back(binary);
        traverse(binary->getRight());
    }

    shared_ptr <Node> createCopy(shared_ptr <Node> original) {
        auto *binary = dynamic_cast<BinaryOperation *>(original.get());
        if (binary == nullptr) {
            auto *unary = dynamic_cast<UnaryOperation *>(original.get());
            if (unary != nullptr) {
                unary->setOperand(createCopy(unary->getOperand()));
                auto *cosine = dynamic_cast<Cosine *>(original.get());
                if (cosine != nullptr) {
                    return shared_ptr<Cosine>(new Cosine(*cosine));
                }
                auto *cube = dynamic_cast<Cube *>(original.get());
                if (cube != nullptr) {
                    return shared_ptr<Cube>(new Cube(*cube));
                }
                auto *cubeRoot = dynamic_cast<CubeRoot *>(original.get());
                if (cubeRoot != nullptr) {
                    return shared_ptr<CubeRoot>(new CubeRoot(*cubeRoot));
                }
                auto *exponentiation = dynamic_cast<Exponentiation *>(original.get());
                if (exponentiation != nullptr) {
                    return shared_ptr<Exponentiation>(new Exponentiation(*exponentiation));
                }
                auto *logarithmBinary = dynamic_cast<LogarithmBinary *>(original.get());
                if (logarithmBinary != nullptr) {
                    return shared_ptr<LogarithmBinary>(new LogarithmBinary(*logarithmBinary));
                }
                auto *logarithmCommon = dynamic_cast<LogarithmCommon *>(original.get());
                if (logarithmCommon != nullptr) {
                    return shared_ptr<LogarithmCommon>(new LogarithmCommon(*logarithmCommon));
                }
                auto *logarithmNatural = dynamic_cast<LogarithmNatural *>(original.get());
                if (logarithmNatural != nullptr) {
                    return shared_ptr<LogarithmNatural>(new LogarithmNatural(*logarithmNatural));
                }
                auto *sine = dynamic_cast<Sine *>(original.get());
                if (sine != nullptr) {
                    return shared_ptr<Sine>(new Sine(*sine));
                }
                auto *square = dynamic_cast<Square *>(original.get());
                if (square != nullptr) {
                    return shared_ptr<Square>(new Square(*square));
                }
                auto *squareRoot = dynamic_cast<SquareRoot *>(original.get());
                if (squareRoot != nullptr) {
                    return shared_ptr<SquareRoot>(new SquareRoot(*squareRoot));
                }
                auto *squareRootNegative = dynamic_cast<SquareRootNegative *>(original.get());
                if (squareRootNegative != nullptr) {
                    return shared_ptr<SquareRootNegative>(new SquareRootNegative(*squareRootNegative));
                }
                auto *tangent = dynamic_cast<Tangent *>(original.get());
                if (tangent != nullptr) {
                    return shared_ptr<Tangent>(new Tangent(*tangent));
                }
            }
            auto *constant = dynamic_cast<Constant *>(original.get());
            if (constant != nullptr) {
                Constant *constantCopy = new Constant(*constant);
                constants.insert(constantCopy);
                return shared_ptr<Constant>(constantCopy);
            }
            auto *variable = dynamic_cast<Variable *>(original.get());
            if (variable != nullptr) {
                Variable *variableCopy = new Variable(*variable);
                variableNames[variableCopy->toString()] = variableCopy;
                return shared_ptr<Variable>(variableCopy);
            }
            auto *number = dynamic_cast<Number *>(original.get());
            if (number != nullptr && variable == nullptr) {
                Number *numberCopy = new Number(*number);
                numbers.push_back(numberCopy);
                return shared_ptr<Number>(numberCopy);
            }
            auto *wrapper = dynamic_cast<Wrapper *>(original.get());
            if (wrapper != nullptr) {
                Wrapper *wrapperCopy = new Wrapper(*wrapper);
                wrapperCopy->setNode(createCopy(wrapper->getNode()));
                return shared_ptr<Wrapper>(wrapperCopy);
            }
        }
        binary->setLeft(createCopy(binary->getLeft()));
        binary->setRight(createCopy(binary->getRight()));
        auto *addition = dynamic_cast<Addition *>(original.get());
        if (addition != nullptr) {
            auto additionCopy = new Addition(*addition);
            binaryOperators.push_back(additionCopy);
            return shared_ptr<Addition>(additionCopy);
        }
        auto *division = dynamic_cast<Division *>(original.get());
        if (division != nullptr) {
            auto divisionCopy = new Division(*division);
            binaryOperators.push_back(divisionCopy);
            return shared_ptr<Division>(divisionCopy);
        }
        auto *multiplication = dynamic_cast<Multiplication *>(original.get());
        if (multiplication != nullptr) {
            auto multiplicationCopy = new Multiplication(*multiplication);
            binaryOperators.push_back(multiplicationCopy);
            return shared_ptr<Multiplication>(multiplicationCopy);
        }
        auto *power = dynamic_cast<Power *>(original.get());
        if (power != nullptr) {
            auto powerCopy = new Power(*power);
            binaryOperators.push_back(powerCopy);
            return shared_ptr<Power>(powerCopy);
        }
        auto *subtraction = dynamic_cast<Subtraction *>(original.get());
        if (subtraction != nullptr) {
            auto subtractionCopy = new Subtraction(*subtraction);
            binaryOperators.push_back(subtractionCopy);
            return shared_ptr<Subtraction>(subtractionCopy);
        }
        return original;
    }
};

#endif