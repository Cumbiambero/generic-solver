#ifndef GENERIC_SOLVER_VARIABLE_SWAPPER_HPP
#define GENERIC_SOLVER_VARIABLE_SWAPPER_HPP

#include "changer-base.hpp"

class VariableSwapper final : public Changer {
public:
    VariableSwapper() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::VARIABLE_SWAPPER;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto variables = result.getVariables();
        if (variables.size() < 2) {
            return result; // Can't swap with less than 2 variables
        }
        
        auto binaryOps = result.getBinaryOperators();
        for (auto* binOp : binaryOps) {
            if (coin_->toss()) {
                auto temp = binOp->getLeft();
                binOp->setLeft(binOp->getRight());
                binOp->setRight(temp);
            }
        }
        
        return result;
    }
};

#endif
