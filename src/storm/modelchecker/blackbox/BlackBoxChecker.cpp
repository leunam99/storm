//
// Created by Maximilian Kamps on 25.05.23.
//

#include "BlackBoxChecker.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ModelType, typename StateType>
BlackBoxChecker<ModelType, StateType>::BlackboxMDP(storm::prism::Program const& program): blackboxMDP(std::make_shared<BlackboxMDP<StateType>((BlackboxMDP<StateType>) (BlackboxWrapperOnWhitebox<StateType, ValueType>(program)))) {
    // intentionally empty
}

template<typename ModelType, typename StateType>
bool BlackBoxChecker<ModelType, StateType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
    // TODO implement actual check
    return true
}

template<typename ModelType, typename StateType>
std::unique_ptr<CheckResult> BlackBoxChecker<ModelType, StateType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
    
    // TODO return actual result when it can be computed
    return  std::make_unique(storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>(0, 1))
}


} //namespace blackbox
} //namespace modelchecker
} //namespace storm

//TODO einzelnd kompilieren
//TODO über storm executeable ausführbar sein
