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
    Formula(N &node, const vector<Variable> &vars)
            : root(make_shared<Wrapper>(Wrapper(node))) {
        init(vars);
    }

    Formula(const Formula &copy)
            : constants(copy.constants),
              binaryOperators(copy.binaryOperators),
              numbers(copy.numbers),
              variablePositions(copy.variablePositions),
              variableNames(copy.variableNames),
              root(shared_ptr(copy.root)) {}

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
                variablePositions[i]->setValue(values[i]);
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
        std::vector<Variable> variables;
        if (!variableNames.empty()) {
            for (const auto &[_, value]: variableNames) {
                variables.push_back(*value);
            }
        }
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
};

#endif