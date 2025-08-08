#ifndef GENERIC_SOLVER_STRUCTURE_MUTATOR_HPP
#define GENERIC_SOLVER_STRUCTURE_MUTATOR_HPP

#include "changer-base.hpp"

class StructureMutator final : public Changer {
public:
    StructureMutator() : Changer() {}

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::STRUCTURE_MUTATOR;
    }

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        
        auto numbers = result.getNumbers();
        for (auto* number : numbers) {
            if (coin_->toss()) {
                const auto simpleValues = {0.0L, 1.0L, 2.0L, 0.5L, -1.0L};
                auto randomIndex = coin_->toss() ? 0 : 4;
                auto newValue = *std::next(simpleValues.begin(), randomIndex);
                number->setValue(newValue);
            }
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
