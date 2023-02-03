#ifndef GENERIC_SOLVER_CHANGERS_PURGER_HPP
#define GENERIC_SOLVER_CHANGERS_PURGER_HPP

#include "changer-base.hpp"
#include "../creators/operation-producer.hpp"

class Purger : public Changer {
public:
    Purger() : Changer(), operationProducer(make_shared<OperationProducer>()) {}

    template<typename C, typename R>
    explicit Purger(C &coin, R &randomNumber) : Changer(coin), operationProducer(
            make_shared<OperationProducer>(OperationProducer(randomNumber))) {}

    Formula change(Formula &formula) override {
        formula.setRoot(operationProducer->produce(formula.getVariables()));
        return formula;
    }

    ChangerType getType() override {
        return ChangerType::PURGER;
    }
private:
    shared_ptr<OperationProducer> operationProducer;
};

#endif