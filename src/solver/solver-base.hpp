#ifndef GENERIC_SOLVER_CHANGERS_BASE_HPP
#define GENERIC_SOLVER_CHANGERS_BASE_HPP

#include "formula.hpp"
#include "../utils/arbitrary.hpp"
#include "../utils/config.hpp"
#include "changers/changers.hpp"
#include "creators/creators.hpp"
#include <unordered_map>

enum class SolverState {
    READY,
    RUNNING,
    DONE
};

class ChangerPicker {
public:
    ChangerPicker() : coin_(make_shared<RandomCoin>()), randomNumber_(make_shared<AlmostRandomNumber>()) {
        init();
    }

    [[nodiscard]] Changer* pickChanger(ChangerType changerType) const {
        return coin_->toss() ? changers_.at(changerType).get() : pickRandomChanger();
    }

    [[nodiscard]] Changer* pickRandomChanger() const {
        if (changers_.empty()) return nullptr;
        
        const auto index = randomNumber_->calculate(0, static_cast<int>(changers_.size()) - 1);
        auto it = changers_.begin();
        std::advance(it, static_cast<size_t>(index) % changers_.size());
        return it->second.get();
    }

private:
    std::unordered_map<ChangerType, std::unique_ptr<Changer>> changers_;
    shared_ptr<Coin> coin_;
    shared_ptr<RandomNumber> randomNumber_;

    void init() {
        initChangers();
    }

    void initChangers() {
        changers_[ChangerType::FLIPPER] = make_unique<Flipper>();
        changers_[ChangerType::INCREMENTOR_BY_ONE] = make_unique<IncrementorByOne>();
        changers_[ChangerType::INCREMENTOR_BY_DOUBLING] = make_unique<IncrementorByDoubling>();
        changers_[ChangerType::INCREMENTOR_BY_FRAGMENT] = make_unique<IncrementorByFragment>();
        changers_[ChangerType::REDUCER_BY_ONE] = make_unique<ReducerByOne>();
        changers_[ChangerType::REDUCER_BY_HALVING] = make_unique<ReducerByHalving>();
        changers_[ChangerType::REDUCER_BY_FRAGMENT] = make_unique<ReducerByFragment>();
        changers_[ChangerType::NUMBER_INSERTER] = make_unique<NumberInserter>();
        changers_[ChangerType::OPERATION_REPLACER] = make_unique<OperationReplacer>();
        changers_[ChangerType::PURGER] = make_unique<Purger>();
        changers_[ChangerType::MERGER] = make_unique<OperationReplacer>(); // this is intentional
        changers_[ChangerType::SIMPLIFIER] = make_unique<Simplifier>();
        changers_[ChangerType::FUNCTION_TRANSFORMER] = make_unique<FunctionTransformer>();
        changers_[ChangerType::VARIABLE_SWAPPER] = make_unique<VariableSwapper>();
        changers_[ChangerType::STRUCTURE_MUTATOR] = make_unique<StructureMutator>();
        changers_[ChangerType::ADAPTIVE_MUTATOR] = make_unique<AdaptiveMutator>();
        changers_[ChangerType::FILTER_RELATIONSHIP_MUTATOR] = make_unique<FilterRelationshipMutator>();
        changers_[ChangerType::EXPONENTIAL_PATTERN_ENHANCER] = make_unique<ExponentialPatternEnhancer>();
        changers_[ChangerType::POWER_RELATIONSHIP_PROMOTER] = make_unique<PowerRelationshipPromoter>();
        changers_[ChangerType::PRECISION_TUNER] = make_unique<PrecisionTuner>();
        changers_[ChangerType::RANGE_OPTIMIZER] = make_unique<RangeOptimizer>();
        changers_[ChangerType::NONLINEARITY_INJECTOR] = make_unique<NonlinearityInjector>();
    }
};

#endif