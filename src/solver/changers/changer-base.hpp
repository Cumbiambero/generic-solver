#ifndef GENERIC_SOLVER_CHANGERS_CHANGER_BASE_HPP
#define GENERIC_SOLVER_CHANGERS_CHANGER_BASE_HPP

#include "../solver-base.hpp"

class Changer {
public:
    Changer() : coin(make_shared<RandomCoin>()) {}

    template<typename C>
    explicit Changer(C &coin) : coin(make_shared<C>(coin)) {}

    virtual ~Changer() = default;

    virtual void change(Formula &formula) = 0;

protected:
    shared_ptr<Coin> coin;
};

#endif