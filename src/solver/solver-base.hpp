#ifndef GENERIC_SOLVER_CHANGERS_BASE_HPP
#define GENERIC_SOLVER_CHANGERS_BASE_HPP

#include "formula.hpp"
#include "../utils/arbitrary.hpp"
#include "../utils/config.hpp"
#include "changers/changers.hpp"
#include "creators/creators.hpp"

enum class SolverState {
    READY,
    RUNNING,
    DONE
};

class ChangerPicker {
public:
    ChangerPicker() : coin(make_shared<RandomCoin>()), randomNumber(make_shared<AlmostRandomNumber>()) {
        init();
    }

    Changer *pickChanger(const ChangerType changerType) {
        return coin->toss() ? changers[changerType].get() : pickRandomChanger();
    }

    Changer *pickRandomChanger() {
        auto it = changers.begin();
        std::advance(it, randomNumber->calculate(0, changers.size() - 1) % changers.size());
        return it->second.get();
    }

private:
    map<ChangerType, unique_ptr<Changer>> changers;
    shared_ptr<RandomNumber> randomNumber;
    shared_ptr<Coin> coin;

    void init() {
        initChangers();
    }

    void initChangers() {
        changers[ChangerType::FLIPPER] = make_unique<Flipper>();
        changers[ChangerType::INCREMENTOR_BY_ONE] = make_unique<IncrementorByOne>();
        changers[ChangerType::INCREMENTOR_BY_DOUBLING] = make_unique<IncrementorByDoubling>();
        changers[ChangerType::INCREMENTOR_BY_FRAGMENT] = make_unique<IncrementorByFragment>();
        changers[ChangerType::REDUCER_BY_ONE] = make_unique<ReducerByOne>();
        changers[ChangerType::REDUCER_BY_HALVING] = make_unique<ReducerByHalving>();
        changers[ChangerType::REDUCER_BY_FRAGMENT] = make_unique<ReducerByFragment>();
        changers[ChangerType::NUMBER_INSERTER] = make_unique<NumberInserter>();
        changers[ChangerType::OPERATION_REPLACER] = make_unique<OperationReplacer>();
        changers[ChangerType::MERGER] = make_unique<OperationReplacer>(); // this is intentional
    }
};

#endif