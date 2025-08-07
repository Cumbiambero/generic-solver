#ifndef GENERIC_SOLVER_CREATORS_CREATOR_BASE_HPP
#define GENERIC_SOLVER_CREATORS_CREATOR_BASE_HPP

#include "operation-producer.hpp"
#include "../changers/changers.hpp"

class Creator : public Changer {
public:
    Creator() : coin_(make_shared<RandomCoin>()), operationProducer(make_shared<OperationProducer>()) {}

    template<typename C, typename R>
    explicit Creator(C &coin, R &randomNumber) : coin_(make_shared<C>(coin)), operationProducer(
            make_shared<OperationProducer>(OperationProducer(randomNumber))) {}

    ~Creator() override = default;

    [[nodiscard]] Formula change(const Formula& formula) const override = 0;

    [[nodiscard]] ChangerType getType() const noexcept override = 0;

protected:
    shared_ptr<Coin> coin_;
    shared_ptr<OperationProducer> operationProducer;
};

#endif
