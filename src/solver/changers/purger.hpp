#ifndef GENERIC_SOLVER_CHANGERS_PURGER_HPP
#define GENERIC_SOLVER_CHANGERS_PURGER_HPP

#include "changer-base.hpp"
#include "../creators/operation-producer.hpp"

class Purger : public Changer {
public:
    Purger() : Changer(), operationProducer(make_shared<OperationProducer>()) {}

        template<typename C, typename R>
        explicit Purger(C &coin, R &randomNumber)
                : Changer(coin),
                    operationProducer(make_shared<OperationProducer>(std::make_shared<R>(randomNumber))) {}

    [[nodiscard]] Formula change(const Formula& formula) const override {
        Formula result = formula;
        result.setRoot(operationProducer->produce(formula.getVariables()));
        return result;
    }

    [[nodiscard]] ChangerType getType() const noexcept override {
        return ChangerType::PURGER;
    }
private:
    shared_ptr<OperationProducer> operationProducer;
};

#endif