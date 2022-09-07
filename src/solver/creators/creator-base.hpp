#ifndef GENERIC_SOLVER_CREATORS_CREATOR_BASE_HPP
#define GENERIC_SOLVER_CREATORS_CREATOR_BASE_HPP

#include "operation-producer.hpp"

class Creator {
public:
    Creator() : coin(make_shared<RandomCoin>()), operationProducer(make_shared<OperationProducer>()) {}

    template<typename C, typename R>
    explicit Creator(C &coin, R &randomNumber) : coin(make_shared<C>(coin)), operationProducer(
            make_shared<OperationProducer>(OperationProducer(randomNumber))) {}

    virtual ~Creator() = default;

protected:
    shared_ptr<Coin> coin;
    shared_ptr<OperationProducer> operationProducer;
};

#endif
