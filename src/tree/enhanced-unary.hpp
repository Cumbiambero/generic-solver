#ifndef GENERIC_SOLVER_ENHANCED_UNARY_HPP
#define GENERIC_SOLVER_ENHANCED_UNARY_HPP

#include "tree-base.hpp"
#include <cmath>

/**
 * Enhanced unary operations for audio/signal processing
 */

class HyperbolicTangent : public UnaryOperation {
public:
    HyperbolicTangent(NodePtr operand) : UnaryOperation("tanh", std::move(operand)) {}
    
    number calculate() const override {
        return std::tanh(operand_->calculate());
    }
    
    string getCppFunction() const override { return "std::tanh"; }
    
    NodePtr clone() const override {
        return make_shared<HyperbolicTangent>(operand_->clone());
    }
};

class HyperbolicSine : public UnaryOperation {
public:
    HyperbolicSine(NodePtr operand) : UnaryOperation("sinh", std::move(operand)) {}
    
    number calculate() const override {
        return std::sinh(operand_->calculate());
    }
    
    string getCppFunction() const override { return "std::sinh"; }
    
    NodePtr clone() const override {
        return make_shared<HyperbolicSine>(operand_->clone());
    }
};

class AbsoluteValue : public UnaryOperation {
public:
    AbsoluteValue(NodePtr operand) : UnaryOperation("abs", std::move(operand)) {}
    
    number calculate() const override {
        return std::abs(operand_->calculate());
    }
    
    string getCppFunction() const override { return "std::abs"; }
    
    NodePtr clone() const override {
        return make_shared<AbsoluteValue>(operand_->clone());
    }
};

class Sigmoid : public UnaryOperation {
public:
    Sigmoid(NodePtr operand) : UnaryOperation("sigmoid", std::move(operand)) {}
    
    number calculate() const override {
        const auto x = operand_->calculate();
        return 1.0L / (1.0L + std::exp(-x));
    }
    
    string getCppFunction() const override { 
        return "([](long double x) { return 1.0L / (1.0L + std::exp(-x)); })";
    }
    
    NodePtr clone() const override {
        return make_shared<Sigmoid>(operand_->clone());
    }
};

class SoftSaturation : public UnaryOperation {
public:
    SoftSaturation(NodePtr operand) : UnaryOperation("sat", std::move(operand)) {}
    
    number calculate() const override {
        const auto x = operand_->calculate();
        return x / (1.0L + std::abs(x));
    }
    
    string getCppFunction() const override { 
        return "([](long double x) { return x / (1.0L + std::abs(x)); })";
    }
    
    NodePtr clone() const override {
        return make_shared<SoftSaturation>(operand_->clone());
    }
};

#endif
