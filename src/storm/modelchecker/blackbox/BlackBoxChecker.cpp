//
// Created by Maximilian Kamps on 25.05.23.
//
#include <fstream>
#include <string>

#include "BlackBoxChecker.h"

#include "storm/modelchecker/blackbox/BlackBoxExplorer.h"
#include "storm/modelchecker/blackbox/BMdp.h"
#include "storm/modelchecker/blackbox/bound-functions/BoundFunc.h"
#include "storm/modelchecker/blackbox/deltaDistribution/DeltaDistribution.h"
#include "storm/modelchecker/blackbox/EMdpToDot.h"
#include "storm/modelchecker/blackbox/EMdp2BMdp.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BlackboxSettings.h"


namespace storm {
namespace modelchecker {
namespace blackbox {



template<typename ModelType, typename StateType>
BlackBoxChecker<ModelType, StateType>::BlackBoxChecker(storm::prism::Program const& program): blackboxMDP(nullptr){
    BlackboxWrapperOnWhitebox<StateType, ValueType> whiteboxWrapper(program);
    auto ptr = std::make_shared<BlackboxWrapperOnWhitebox<StateType, ValueType>>(program);
    blackboxMDP = std::static_pointer_cast<BlackboxMDP<StateType>>(ptr);    
}

template<typename ModelType, typename StateType>
bool BlackBoxChecker<ModelType, StateType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
    // TODO implement actual check
    return true;
}

template<typename ModelType, typename StateType>
std::unique_ptr<CheckResult> BlackBoxChecker<ModelType, StateType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) { 
    auto blackboxSettings = storm::settings::getModule<storm::settings::modules::BlackboxSettings>();
    // cli arguments simulate
    uint_fast64_t maxIterations = blackboxSettings.getMaxIterations();
    std::seed_seq seedSimHeuristic = blackboxSettings.getSimHeuristicSeed();
    uint_fast64_t simulationsPerIter = blackboxSettings.getNumberOfSamplingsPerSimulationStep();
    heuristicSim::HeuristicSimType heuristicSimType = blackboxSettings.getSimulationHeuristicType();
    // cli arguments infer
    BoundFuncType boundFuncType = blackboxSettings.getBoundFuncType();
    DeltaDistType deltaDistType = blackboxSettings.getDeltaDistType();    
    double delta = blackboxSettings.getDelta();
    // cli arguments other
    double eps = blackboxSettings.getPrecision();
    std::string eMdpFilePath = "eMdp"; // TODO get from cli

    // init objects for algorithm
    EMdp<BlackboxStateType> eMDP;
    auto heuristicSim(std::static_pointer_cast<heuristicSim::HeuristicSim<StateType, ValueType>>(std::make_shared<heuristicSim::NaiveHeuristicSim<StateType, ValueType>>(blackboxMDP, seedSimHeuristic)));
    BlackBoxExplorer<StateType, ValueType> blackboxExplorer(blackboxMDP, heuristicSim);
    auto boundFunc = getBoundFunc(boundFuncType);
    auto deltaDist = getDeltaDistribution(deltaDistType);
    std::pair<double, double> valueBounds = std::make_pair(0, 1);

    // init objects for output generation
    EMdpDotGenerator<BlackboxStateType> eMdpDotGenerator;
    std::string eMdpFilename;
    std::ofstream eMdpFile;
    // TODO init objects for bMDP output

    // run 3 step algorithm
    uint_fast64_t iterCount = 0;
    while (eps < valueBounds.second - valueBounds.first && iterCount < maxIterations) {
        iterCount++;
        
        // simulate
        blackboxExplorer.performExploration(eMDP, simulationsPerIter);
        // create simulate output
        eMdpFile.open(eMdpFilePath + "_" + std::to_string(iterCount) + ".dot");
        eMdpDotGenerator.convert(eMDP, eMdpFile);
        eMdpFile.close();

        // infer 
        auto bMDP = infer(eMDP, boundFunc, deltaDist, blackboxMDP->getPmin(), delta, !blackboxMDP->isGreybox());
        // TODO create infer output

        // value approximation (implemented some time in future)
    }

    // TODO return actual result when it can be computed
    return  std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(0, 1);
}

template class BlackBoxChecker<storm::models::sparse::Mdp<double>, uint32_t>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm

//TODO einzelnd kompilieren
//TODO über storm executeable ausführbar sein
