//
// Created by Maximilian Kamps on 25.05.23.
//

#include "BlackBoxChecker.h"

#include "storm/modelchecker/blackbox/blackBoxExplorer.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ModelType, typename StateType>
BlackBoxChecker<ModelType, StateType>::BlackBoxChecker(storm::prism::Program const& program): blackboxMDP((std::shared_ptr<BlackboxMDP<StateType>>(new BlackboxWrapperOnWhitebox<StateType, ValueType>(program)))) {
    // std::shared_ptr<BlackBoxMDP<StateType>> ptr(new BlackboxWrapperOnWhitebox<StateType, ValueType>(program));
    // blackboxMDP = ptr;
}

template<typename ModelType, typename StateType>
bool BlackBoxChecker<ModelType, StateType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) {
    // TODO implement actual check
    return true;
}

template<typename ModelType, typename StateType>
std::unique_ptr<CheckResult> BlackBoxChecker<ModelType, StateType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
    EMdp<StateType> eMDP;
    std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim(new heuristicSim::NaiveHeuristicSim<StateType, ValueType>(blackboxMDP)); // TODO get heuristic via cli + seed
    BlackBoxExplorer<StateType, ValueType> blackboxExplorer(blackboxMDP, heuristicSim);

    // TODO return actual result when it can be computed
    return  std::make_unique(storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>(0, 1));
}


} //namespace blackbox
} //namespace modelchecker
} //namespace storm

//TODO einzelnd kompilieren
//TODO über storm executeable ausführbar sein
