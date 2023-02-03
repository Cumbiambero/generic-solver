#ifndef GENERIC_SOLVER_CHANGERS_CHANGER_BASE_HPP
#define GENERIC_SOLVER_CHANGERS_CHANGER_BASE_HPP

enum class ChangerType {
    FLIPPER,
    INCREMENTOR_BY_DOUBLING,
    INCREMENTOR_BY_FRAGMENT,
    INCREMENTOR_BY_ONE,
    MERGER,
    NUMBER_INSERTER,
    OPERATION_REPLACER,
    PURGER,
    REDUCER_BY_FRAGMENT,
    REDUCER_BY_HALVING,
    REDUCER_BY_ONE
};

class Changer {
public:
    Changer() : coin(make_shared<RandomCoin>()) {}

    template<typename C>
    explicit Changer(C &coin) : coin(make_shared<C>(coin)) {}

    virtual ~Changer() = default;

    virtual Formula change(Formula &formula) = 0;

    virtual ChangerType getType() = 0;

protected:
    shared_ptr<Coin> coin;
};

#endif