#ifndef GENERIC_SOLVER_FORMULA_HPP
#define GENERIC_SOLVER_FORMULA_HPP

#include "../tree/tree.hpp"
#include <unordered_map>
#include <unordered_set>
#include <shared_mutex>
#include <array>
#include <iostream>
#include <utility>

class Formula {
public:
    template<typename N>
    Formula(N& node, Variable params...) : Formula(node, vector<Variable>{std::move(params)}) {}

    template<typename N>
    Formula(N& node, const vector<Variable>& variables)
            : variables_(variables), root_(make_shared<Wrapper>(Wrapper(node))) {
        init(variables);
    }

    Formula(const Formula& copy)
            : variables_(copy.variables_),
              root_(createCopy(copy.root_)) {
        init(variables_);
    }

    Formula(Formula&& other) noexcept = default;
    Formula& operator=(Formula&& other) noexcept = default;
    Formula& operator=(const Formula& other) {
        if (this != &other) {
            variables_ = other.variables_;
            root_ = createCopy(other.root_);
            init(variables_);
        }
        return *this;
    }

    virtual ~Formula() = default;

    template<std::size_t N>
    [[nodiscard]] number evaluate(const std::array<number, N>& params) const {
        if (variableNames_.size() != N) {
            throw std::invalid_argument("Parameter count mismatch");
        }

        std::shared_lock lock(mutex_);
        if (!variablePositions_.empty() && variablePositions_.size() == params.size()) {
            for (std::size_t i = 0; i < N; i++) {
                auto it = variablePositions_.find(i);
                if (it != variablePositions_.end() && it->second) {
                    it->second->setValue(params[i]);
                }
            }
        }
        const number result = root_->calculate();
        return std::isfinite(result) ? result : std::numeric_limits<number>::lowest();
    }

    [[nodiscard]] number evaluate(number param1) const {
        return evaluate(std::array<number, 1>{param1});
    }

    [[nodiscard]] number evaluate(number param1, number param2) const {
        return evaluate(std::array<number, 2>{param1, param2});
    }

    [[nodiscard]] number evaluate(number param1, number param2, number param3) const {
        return evaluate(std::array<number, 3>{param1, param2, param3});
    }

    [[nodiscard]] number evaluate(number param1, number param2, number param3, number param4) const {
        return evaluate(std::array<number, 4>{param1, param2, param3, param4});
    }

    [[nodiscard]] number evaluate(const vector<number>& params) const {
        if (variableNames_.size() != params.size()) {
            throw std::invalid_argument("Parameter count mismatch");
        }

        std::shared_lock lock(mutex_);
        if (!variablePositions_.empty() && variablePositions_.size() == params.size()) {
            for (std::size_t i = 0; i < params.size(); i++) {
                auto it = variablePositions_.find(i);
                if (it != variablePositions_.end() && it->second) {
                    it->second->setValue(params[i]);
                }
            }
        }
        const number result = root_->calculate();
        return std::isfinite(result) ? result : std::numeric_limits<number>::lowest();
    }

    [[nodiscard]] string toString() const { 
        root_->simplify();
        return root_->toString();
    }

    [[nodiscard]] NodePtr getRoot() const noexcept { return root_; }
    void setRoot(NodePtr rootNode) noexcept { root_ = std::move(rootNode); }

    [[nodiscard]] const vector<BinaryOperation*>& getBinaryOperators() const noexcept { 
        return binaryOperators_; 
    }

    [[nodiscard]] const vector<Variable>& getVariables() const noexcept {
        return variables_;
    }

    [[nodiscard]] const vector<Number*>& getNumbers() const noexcept {
        return numbers_;
    }

    [[nodiscard]] string toCppCode() const {
        return root_ ? root_->toCppCode() : "0.0L";
    }

    [[nodiscard]] bool operator==(const Formula& rhs) const {
        return root_->toString() == rhs.root_->toString();
    }

    [[nodiscard]] bool operator!=(const Formula& rhs) const {
        return !(*this == rhs);
    }

private:
    mutable std::shared_mutex mutex_;
    std::unordered_set<Constant*> constants_;
    vector<BinaryOperation*> binaryOperators_;
    vector<Number*> numbers_;
    std::unordered_map<std::size_t, Variable*> variablePositions_;
    std::unordered_map<string, Variable*> variableNames_;
    vector<Variable> variables_;
    NodePtr root_;

    void init(const vector<Variable>& vars) {
        variableNames_.clear();
        variablePositions_.clear();
        binaryOperators_.clear();
        numbers_.clear();
        constants_.clear();
        
        traverse(root_);
        
        for (std::size_t i = 0; i < vars.size(); i++) {
            const auto& variable = vars[i];
            const auto it = variableNames_.find(variable.toString());
            if (it != variableNames_.end()) {
                variablePositions_[i] = it->second;
            }
        }
    }

    void traverse(const NodePtr& current) {
        if (!current) return;
        
        if (auto* binary = dynamic_cast<BinaryOperation*>(current.get())) {
            traverse(binary->getLeft());
            traverse(binary->getRight());
            binaryOperators_.push_back(binary);
            return;
        }

        if (auto* unary = dynamic_cast<UnaryOperation*>(current.get())) {
            traverse(unary->getOperand());
            return;
        }

        if (auto* constant = dynamic_cast<Constant*>(current.get())) {
            constants_.insert(constant);
            return;
        }

        if (auto* variable = dynamic_cast<Variable*>(current.get())) {
            variableNames_[variable->toString()] = variable;
            return;
        }

        if (auto* number = dynamic_cast<Number*>(current.get())) {
            // Only add if it's not a variable
            if (!dynamic_cast<Variable*>(current.get())) {
                numbers_.push_back(number);
            }
            return;
        }

        if (auto* wrapper = dynamic_cast<Wrapper*>(current.get())) {
            traverse(wrapper->getNode());
            return;
        }
    }

    [[nodiscard]] NodePtr createCopy(const NodePtr& original) {
        if (!original) return nullptr;

        // Handle binary operations
        if (auto* binary = dynamic_cast<BinaryOperation*>(original.get())) {
            return createBinaryCopy(binary);
        }

        // Handle unary operations
        if (auto* unary = dynamic_cast<UnaryOperation*>(original.get())) {
            return createUnaryCopy(unary);
        }

        // Handle constants
        if (auto* constant = dynamic_cast<Constant*>(original.get())) {
            constants_.insert(constant);
            return original; // Constants are immutable, can share
        }

        // Handle variables
        if (auto* variable = dynamic_cast<Variable*>(original.get())) {
            const auto variableName = variable->toString();
            auto variableCopy = make_shared<Variable>(variableName);
            variableNames_[variableName] = variableCopy.get();
            return variableCopy;
        }

        // Handle numbers
        if (auto* number = dynamic_cast<Number*>(original.get())) {
            auto copy = make_shared<Number>(number->calculate());
            numbers_.push_back(copy.get());
            return copy;
        }

        // Handle wrappers
        if (auto* wrapper = dynamic_cast<Wrapper*>(original.get())) {
            return make_shared<Wrapper>(createCopy(wrapper->getNode()));
        }

        return original;
    }

    [[nodiscard]] NodePtr createBinaryCopy(BinaryOperation* binary) {
        NodePtr leftCopy = createCopy(binary->getLeft());
        NodePtr rightCopy = createCopy(binary->getRight());
        
        NodePtr copy;
        if (dynamic_cast<Addition*>(binary)) {
            copy = make_shared<Addition>(leftCopy, rightCopy);
        } else if (dynamic_cast<Subtraction*>(binary)) {
            copy = make_shared<Subtraction>(leftCopy, rightCopy);
        } else if (dynamic_cast<Multiplication*>(binary)) {
            copy = make_shared<Multiplication>(leftCopy, rightCopy);
        } else if (dynamic_cast<Division*>(binary)) {
            copy = make_shared<Division>(leftCopy, rightCopy);
        } else if (dynamic_cast<Power*>(binary)) {
            copy = make_shared<Power>(leftCopy, rightCopy);
        } else {
            // Fallback for unknown binary operations
            return binary->shared_from_this();
        }
        
        binaryOperators_.push_back(static_cast<BinaryOperation*>(copy.get()));
        return copy;
    }

    [[nodiscard]] NodePtr createUnaryCopy(UnaryOperation* unary) {
        NodePtr operandCopy = createCopy(unary->getOperand());
        
        NodePtr copy;
        if (dynamic_cast<Sine*>(unary)) {
            copy = make_shared<Sine>(operandCopy);
        } else if (dynamic_cast<Cosine*>(unary)) {
            copy = make_shared<Cosine>(operandCopy);
        } else if (dynamic_cast<Tangent*>(unary)) {
            copy = make_shared<Tangent>(operandCopy);
        } else if (dynamic_cast<Square*>(unary)) {
            copy = make_shared<Square>(operandCopy);
        } else if (dynamic_cast<Cube*>(unary)) {
            copy = make_shared<Cube>(operandCopy);
        } else if (dynamic_cast<SquareRoot*>(unary)) {
            copy = make_shared<SquareRoot>(operandCopy);
        } else if (dynamic_cast<SquareRootNegative*>(unary)) {
            copy = make_shared<SquareRootNegative>(operandCopy);
        } else if (dynamic_cast<CubeRoot*>(unary)) {
            copy = make_shared<CubeRoot>(operandCopy);
        } else if (dynamic_cast<LogarithmNatural*>(unary)) {
            copy = make_shared<LogarithmNatural>(operandCopy);
        } else if (dynamic_cast<LogarithmCommon*>(unary)) {
            copy = make_shared<LogarithmCommon>(operandCopy);
        } else if (dynamic_cast<LogarithmBinary*>(unary)) {
            copy = make_shared<LogarithmBinary>(operandCopy);
        } else if (dynamic_cast<Exponentiation*>(unary)) {
            copy = make_shared<Exponentiation>(operandCopy);
        } else {
            // Fallback for unknown unary operations
            return unary->shared_from_this();
        }
        
        return copy;
    }
};

#endif