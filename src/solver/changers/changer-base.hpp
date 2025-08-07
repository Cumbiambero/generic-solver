#ifndef GENERIC_SOLVER_CHANGERS_CHANGER_BASE_HPP
#define GENERIC_SOLVER_CHANGERS_CHANGER_BASE_HPP

#include "../formula.hpp"
#include "../../utils/arbitrary.hpp"

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
    Changer() : coin_(make_shared<RandomCoin>()) {}

    template<typename C>
    explicit Changer(C& coin) : coin_(make_shared<C>(coin)) {}

    virtual ~Changer() = default;

    // Delete copy constructor and assignment to avoid issues with inheritance
    Changer(const Changer&) = delete;
    Changer& operator=(const Changer&) = delete;
    
    // Allow move semantics
    Changer(Changer&&) = default;
    Changer& operator=(Changer&&) = default;

    [[nodiscard]] virtual Formula change(const Formula& formula) const = 0;
    [[nodiscard]] virtual ChangerType getType() const noexcept = 0;

protected:
    shared_ptr<Coin> coin_;
};

#endif