//
// Created by Maximilian Kamps on 25.05.23.
//
#include <fstream>
#include <string>

#include "BlackBoxChecker.h"

#include "storm/modelchecker/blackbox/BlackBoxExplorer.h"
#include "storm/modelchecker/blackbox/EMdpToDot.h"
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
    std::cout << "foo\n";
    BlackboxWrapperOnWhitebox<StateType, ValueType> whiteboxWrapper(program);
    std::cout << "foo2\n";
    auto ptr = std::make_shared<BlackboxWrapperOnWhitebox<StateType, ValueType>>(program);
    std::cout << "created pointer\n";
    blackboxMDP = std::static_pointer_cast<BlackboxMDP<StateType>>(ptr);    
    std::cout << "blackbox Checker initialized\n";
}

template<typename ModelType, typename StateType>
bool BlackBoxChecker<ModelType, StateType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const {
    // TODO implement actual check
    return true;
}

template<typename ModelType, typename StateType>
std::unique_ptr<CheckResult> BlackBoxChecker<ModelType, StateType>::computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) {
    std::cout << "start algo\n";

    
    // cli arguments
    std::cout << "build cli arguments\n";
    uint_fast64_t maxIterations = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getMaxIterations();
    std::cout << "1\n";
    std::seed_seq seedSimHeuristic = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getSimHeuristicSeed();
    std::cout << "2\n";
    uint_fast64_t simulationsPerIter = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getNumberOfSamplingsPerSimulationStep();
    std::cout << "3\n";
    double eps = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getPrecision();
    std::cout << "4\n";
    heuristicSim::HeuristicSimType heuristicSimType = storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getSimulationHeuristicType();
    std::cout << "5\n";
    std::string eMdpFilePath = "eMdp"; // TODO get from cli
    std::cout << "6\n";

    // init objects for algorithm
    std::cout << "build help objects\n";
    EMdp<BlackboxStateType> eMDP;
    std::cout << "1\n";
    auto ptrTmp = std::make_shared<heuristicSim::NaiveHeuristicSim<StateType, ValueType>>(blackboxMDP, seedSimHeuristic);
    std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim(std::static_pointer_cast<heuristicSim::HeuristicSim<StateType, ValueType>>(ptrTmp));
    std::cout << "2\n";
    BlackBoxExplorer<StateType, ValueType> blackboxExplorer(blackboxMDP, heuristicSim);
    std::cout << "3\n";
    std::pair<double, double> valueBounds = std::make_pair(0, 1);
    std::cout << "4\n";

    // init objects for output generation
    std::cout << "build output objects\n";
    EMdpDotGenerator<BlackboxStateType> eMdpDotGenerator;
    std::cout << "1\n";
    std::string eMdpFilename;
    std::cout << "1\n";
    std::ofstream eMdpFile;
    std::cout << "1\n";

    // run 3 step algorithm
    uint_fast64_t iterCount = 0;
    while (eps < valueBounds.second - valueBounds.first && iterCount < maxIterations) {
        iterCount++;
        std::cout << "iteration " << iterCount << "\n";
        
        // simulate
        blackboxExplorer.performExploration(eMDP, simulationsPerIter);
        // show simulate output
        std::cout << "save emdp nr " << iterCount << "\n";
        eMdpFile.open(eMdpFilePath + "_" + std::to_string(iterCount) + ".dot");
        eMdpDotGenerator.convert(eMDP, eMdpFile);
        eMdpFile.close();

        // infer TODO

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
