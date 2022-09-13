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

    Formula(shared_ptr <Node> node, const vector<Variable> &vars)
            : root(std::move(node)) {
        init(vars);
    }

    Formula(const Formula &copy)
            : root(copy.root), constants(copy.constants), unaryOperators(copy.unaryOperators),
              binaryOperators(copy.binaryOperators), variables(copy.variables), numbers(copy.numbers),
              variablePositions(copy.variablePositions), variableNames(copy.variableNames) {}

    virtual ~Formula() = default;

    number evaluate(number params...) {
        std::vector<number> values = {params};
        size_t numberOfParams = values.size();
        if (variables.size() != numberOfParams) {
            throw invalid_argument("the number of params does not match with the number of params");
        }

        lock.lock();
        for (size_t i = 0; i < numberOfParams; i++) {
            variablePositions[i]->setValue(values[i]);
        }
        number result = root->calculate();
        lock.unlock();

        return result;
    }

    [[nodiscard]] string toString() const { return root->toString(); }

    [[nodiscard]] shared_ptr<Node>& getRoot() { return root; }

    [[nodiscard]] shared_ptr<Node> getRoot() const { return root; }

    vector<BinaryOperation *> &getBinaryOperators() { return binaryOperators; }

    vector<UnaryOperation *> &getUnaryOperators() { return unaryOperators; }

    [[nodiscard]] vector<Variable> getVariables() const { return variables; }

    vector<Number *> getNumbers() {
        return this->numbers;
    }

private:
    mutex lock;
    set<Constant *> constants;
    vector<UnaryOperation *> unaryOperators;
    vector<BinaryOperation *> binaryOperators;
    vector<Variable> variables;
    vector<Number *> numbers;
    map<size_t, Variable *> variablePositions;
    map<string, Variable *> variableNames;
    shared_ptr <Node> root;

    void init(const vector<Variable> &vars) {
        variables.clear();
        variableNames.clear();
        variablePositions.clear();
        traverse(root);
        for (size_t i = 0; i < vars.size(); i++) {
            Variable variable = vars[i];
            variables.push_back(variable);
            variablePositions[i] = variableNames[variable.toString()];
        }
    }

    void traverse(const shared_ptr <Node> &current) {
        auto *binary = dynamic_cast<BinaryOperation *>(current.get());
        if (binary == nullptr) {
            auto *unary = dynamic_cast<UnaryOperation *>(current.get());
            if (unary != nullptr) {
                traverse(unary->getOperand());
                unaryOperators.push_back(unary);
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