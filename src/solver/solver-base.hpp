#ifndef GENERIC_SOLVER_CHANGERS_BASE_HPP
#define GENERIC_SOLVER_CHANGERS_BASE_HPP

#include "formula.hpp"
#include "../utils/arbitrary.hpp"
#include "creators/creators.hpp"
#include "changers/changers.hpp"

enum SolverState {
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

    Changer* pickChanger(ChangerType changerType) {
        return changers[changerType];
    }

    Changer *pickRandomChanger() {
        auto it = changers.begin();
        std::advance(it, randomNumber->calculate(0, changers.size() - 1) % changers.size());
        return it->second;
    }

private:
    map<ChangerType, Changer *> changers;
    map<ChangerType, Creator *> creators;
    shared_ptr<RandomNumber> randomNumber;
    shared_ptr<Coin> coin;

    void init() {
        initChangers();
        initCreators();
    }

    void initChangers() {
        changers[FLIPPER] = make_unique<Flipper>().get();
        changers[INCREMENTOR_BY_ONE] = make_unique<IncrementorByOne>().get();
        changers[INCREMENTOR_BY_DOUBLING] = make_unique<IncrementorByDoubling>().get();
        changers[INCREMENTOR_BY_FRAGMENT] = make_unique<IncrementorByFragment>().get();
        changers[REDUCER_BY_ONE] = make_unique<ReducerByOne>().get();
        changers[REDUCER_BY_HALVING] = make_unique<ReducerByHalving>().get();
        changers[REDUCER_BY_FRAGMENT] = make_unique<ReducerByFragment>().get();
    }

    void initCreators() {
        creators[MERGER] = make_unique<Merger>().get();
        creators[NUMBER_INSERTER] = make_unique<NumberInserter>().get();
        creators[OPERATION_REPLACER] = make_unique<OperationReplacer>().get();
    }
};

#endif