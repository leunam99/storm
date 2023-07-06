//
// Created by Maximilian Kamps on 25.05.23.
//

#include "BlackBoxChecker.h"

#include "storm/modelchecker/blackbox/blackBoxExplorer.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BlackboxSettings.h"


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
    // cli arguments
    uint_fast64_t maxIterations = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getMaxIterations();
    std::seed_seq seedSimHeuristic = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getSimHeuristicSeed();
    uint_fast64_t simulationsPerIt = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getNumberOfSamplingsPerSimulationStep();
    double eps = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getPrecision();
    heuristicSim::HeuristicSimType heuristicSimtype = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getSimulationHeuristicType();

    // init objects for algorithm
    EMdp<StateType> eMDP;
    std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim(new heuristicSim::NaiveHeuristicSim<StateType, ValueType>(blackboxMDP, seedSimHeuristic));
    std::pair<double, double> valueBounds = std::make_pair(0, 1);

    // run 3 step algorithm
    uint64_fast64_t iterCount = 0;
    while (eps < valueBounds.second - valueBounds.first && iterCount < maxIterations) {
        // simulate
        blackBoxExplorer.performExploration(eMDP, simulationsPerIt);
        // simulate output

        // infer TODO

        // value approximation (some time in future)

        iterCount++;
    }

    // TODO return actual result when it can be computed
    return  std::make_unique(storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>(0, 1));
}


} //namespace blackbox
} //namespace modelchecker
} //namespace storm

//TODO einzelnd kompilieren
//TODO über storm executeable ausführbar sein
