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
    Formula(const Formula &copy) {}

    template<typename N>
    Formula(N &node, Variable params...) : Formula(node, vector<Variable>{std::move(params)}) {}

    template<typename N>
    Formula(N &node, const vector<Variable>& vars)
            : root(make_shared<Wrapper>(Wrapper(node))) {
        init(vars);
    }

    Formula(shared_ptr<Node> node, const vector<Variable>& vars)
            : root(std::move((node))) {
        init(vars);
    }

    number evaluate(number params...) {
        std::vector<number> values = {params};
        size_t numberOfParams = values.size();
        if (variables.size() != numberOfParams) {
            throw invalid_argument(
                    "the number of params does not match with the number of params");
        }
        lock.lock();

        for (size_t i = 0; i < numberOfParams; i++) {
            for (auto var: variablePositions[i]) {
                var->setValue(values[i]);
            }
        }
        number result = root->calculate();
        lock.unlock();
        return result;
    }

    string toString() { return root->toString(); }

    shared_ptr<Node> getRoot() { return root; }

    vector<BinaryOperation *> &getBinaryOperators() { return binaryOperators; }

    vector<UnaryOperation *> &getUnaryOperators() { return unaryOperators; }

    vector<Variable> &getVariables() { return variables; }

private:
    mutex lock;
    set<Constant *> constants;
    vector<UnaryOperation *> unaryOperators;
    vector<BinaryOperation *> binaryOperators;
    vector<Variable> variables;
    map<size_t, vector<Variable *>> variablePositions;
    map<string, vector<Variable *>> variableNames;
    shared_ptr<Node> root;

    void init(const vector<Variable> &vars) {
        traverse(root.get());
        for (size_t i = 0; i < vars.size(); i++) {
            Variable variable = vars[i];
            variables.push_back(variable);
            variablePositions[i] = variableNames[variable.toString()];
        }
    }

    void traverse(Node *node) {
        auto *binary = dynamic_cast<BinaryOperation *>(node);
        if (binary == nullptr) {
            auto *unary = dynamic_cast<UnaryOperation *>(node);
            if (unary != nullptr) {
                traverse(unary->getOperand().get());
                unaryOperators.push_back(unary);
            }
            auto *constant = dynamic_cast<Constant *>(node);
            if (constant != nullptr) {
                constants.insert(constant);
            }
            auto *variable = dynamic_cast<Variable *>(node);
            if (variable != nullptr) {
                variableNames[variable->toString()].push_back(variable);
            }
            auto *wrapper = dynamic_cast<Wrapper *>(node);
            if (wrapper != nullptr) {
                traverse(wrapper->getNode().get());
            }
            return;
        }

        traverse(binary->getLeft().get());
        binaryOperators.push_back(binary);
        traverse(binary->getRight().get());
    }
};

#endif