#ifndef GENERIC_SOLVER_CREATORS_CREATOR_BASE_HPP
#define GENERIC_SOLVER_CREATORS_CREATOR_BASE_HPP

#include "operation-producer.hpp"
#include "../changers/changers.hpp"

class Creator : public Changer {
public:
    Creator() : coin(make_shared<RandomCoin>()), operationProducer(make_shared<OperationProducer>()) {}

    template<typename C, typename R>
    explicit Creator(C &coin, R &randomNumber) : coin(make_shared<C>(coin)), operationProducer(
            make_shared<OperationProducer>(OperationProducer(randomNumber))) {}

    ~Creator() override = default;

    Formula change(Formula &formula) override = 0;

    ChangerType getType() override = 0;

protected:
    shared_ptr<Coin> coin;
    shared_ptr<OperationProducer> operationProducer;
};

#endif
