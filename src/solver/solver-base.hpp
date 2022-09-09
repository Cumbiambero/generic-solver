#ifndef GENERIC_SOLVER_CHANGERS_BASE_HPP
#define GENERIC_SOLVER_CHANGERS_BASE_HPP

#include "formula.hpp"
#include "../utils/arbitrary.hpp"
#include "creators/creators.hpp"
#include "changers/changers.hpp"

enum class SolverState {
    BROKEN,
    PAUSED,
    READY,
    RUNNING,
    DONE
};

class ChangerPicker {
public:
    ChangerPicker() : coin(make_shared<RandomCoin>()), randomNumber(make_shared<AlmostRandomNumber>()) {
        init();
    }

    template<typename C, typename N>
    explicit
    ChangerPicker(C coin, N &randomNumber) : coin(make_shared<C>(coin)), randomNumber(make_shared<N>(randomNumber)) {
        init();
    }

    Changer *pickChanger(const ChangerType changerType) {
        return changers[changerType].get();
    }

    Changer *pickRandomChanger() {
        auto it = changers.begin();
        std::advance(it, randomNumber->calculate(0, changers.size() - 1) % changers.size());
        return it->second.get();
    }

private:
    map<ChangerType, unique_ptr<Changer>> changers;
    map<ChangerType, unique_ptr<Creator>> creators;
    shared_ptr<RandomNumber> randomNumber;
    shared_ptr<Coin> coin;

    void init() {
        initChangers();
        initCreators();
    }

    void initChangers() {
        changers[ChangerType::FLIPPER] = make_unique<Flipper>();
        changers[ChangerType::INCREMENTOR_BY_ONE] = make_unique<IncrementorByOne>();
        changers[ChangerType::INCREMENTOR_BY_DOUBLING] = make_unique<IncrementorByDoubling>();
        changers[ChangerType::INCREMENTOR_BY_FRAGMENT] = make_unique<IncrementorByFragment>();
        changers[ChangerType::REDUCER_BY_ONE] = make_unique<ReducerByOne>();
        changers[ChangerType::REDUCER_BY_HALVING] = make_unique<ReducerByHalving>();
        changers[ChangerType::REDUCER_BY_FRAGMENT] = make_unique<ReducerByFragment>();
    }

    void initCreators() {
        creators[ChangerType::MERGER] = make_unique<Merger>();
        creators[ChangerType::NUMBER_INSERTER] = make_unique<NumberInserter>();
        creators[ChangerType::OPERATION_REPLACER] = make_unique<OperationReplacer>();
    }
};

#endif