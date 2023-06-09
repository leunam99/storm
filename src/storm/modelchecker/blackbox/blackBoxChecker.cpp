//
// Created by Maximilian Kamps on 25.05.23.
//

#include "blackBoxChecker.h"
#include "storm/modelchecker/blackbox/blackBoxExplorer.h"

#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BlackboxSettings.h"

#include "storm/models/sparse/Mdp.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "storm/exceptions/InvalidPropertyException.h"


namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ModelType, typename StateType>
blackBoxChecker<ModelType, StateType>::blackBoxChecker(storm::prism::Program const& program): 
    program(program.substituteConstantsFormulas()),
    pathsSampledPerSimulation(storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getNumberOfSamplingsPerSimulationStep()),
    simHeuristic(storm::modelchecker::blackbox::heuristic_simulate::getHeuristicSimFromType(
                 storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getSimulationHeuristic())) {
    // intentionally left empty
}

template<typename ModelType, typename StateType>
bool blackBoxChecker<ModelType, StateType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
    // TODO implement actual check
    return true;
}

template<typename ModelType, typename StateType>
std::unique_ptr<CheckResult> blackBoxChecker<ModelType, StateType>::computeUntilProbabilities(
    Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
    storm::logic::UntilFormula const& untilFormula = checkTask.getFormula();
    storm::logic::Formula const& conditionFormula = untilFormula.getLeftSubformula();
    storm::logic::Formula const& targetFormula = untilFormula.getRightSubformula();
    STORM_LOG_THROW(program.isDeterministicModel() || checkTask.isOptimizationDirectionSet(), storm::exceptions::InvalidPropertyException,
                    "For nondeterministic systems, an optimization direction (min/max) must be given in the property.");

    storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType> explorationInformation(checkTask.isOptimizationDirectionSet() ? checkTask.getOptimizationDirection()
                                                                                                               : storm::OptimizationDirection::Maximize);

    // The first row group starts at action 0.
    explorationInformation.newRowGroup(0);

    std::map<std::string, storm::expressions::Expression> labelToExpressionMapping = program.getLabelToExpressionMapping();
    storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType> stateGeneration(program, explorationInformation,
                                                                                                   conditionFormula.toExpression(program.getManager(), labelToExpressionMapping),
                                                                                                   targetFormula.toExpression(program.getManager(), labelToExpressionMapping));
    storm::modelchecker::blackbox::heuristic_simulate::naiveHeuristicSim heuristic;
    blackBoxExpl.performExploration(stateGeneration, explorationInformation, heuristic, pathsSampledPerSimulation);

    blackBoxExpl.getEmdp()->print();

    return std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(0, 1);
}

template class blackBoxChecker<storm::models::sparse::Mdp<double>, uint32_t>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm

//TODO einzelnd kompilieren
//TODO über storm executeable ausführbar sein
