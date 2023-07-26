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
#include "storm/models/sparse/StandardRewardModel.h"


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

template<typename StateType>
void executeEMdpFlags(settings::modules::BlackboxSettings blackboxSettings, EMdp<StateType> eMDP) {
    if(blackboxSettings.isSetPrintEMdp()) //Print the explored eMDP if flag is set 
            eMDP.print();


    if(blackboxSettings.isSetWriteEMdpToFile()) { //Write to file if flag is set 
        eMDP.eMdpToFile(blackboxSettings.getEMdpOutFileName());
    }

    auto dotGenEMdp = EMdpDotGenerator<StateType>(blackboxSettings.isSetDotIncAct(), 
                                                  blackboxSettings.isSetDotIncSmpl(), 
                                                  blackboxSettings.isSetDotIncLab(), 
                                                  blackboxSettings.isSetDotIncCol());

    if(blackboxSettings.isSetEMdptoDot()) { //Dot coversion if flag is set 
        auto eMDPtemp = (blackboxSettings.getEMdpDotInFileName() == "expl") ? eMDP : eMDP.eMdpFromFile(blackboxSettings.getEMdpDotInFileName());
        if(blackboxSettings.getEMdpDotOutFileName() == "log") {
            dotGenEMdp.convert(eMDPtemp, std::cout);
        } else {
            std::ofstream outFile(blackboxSettings.getEMdpDotOutFileName());
            dotGenEMdp.convert(eMDPtemp, outFile);
            outFile.close();
        }
    }

    if(blackboxSettings.isSetEMdpNeighbToDot()) { //Neighborhood Dot coversion if flag is set 
        auto eMDPtemp = eMDP.eMdpFromFile(blackboxSettings.getEMdpNeighborhoodDotInFileName());
        if(blackboxSettings.getEMdpDotOutFileName() == "log") {
            dotGenEMdp.convertNeighborhood(eMDPtemp, blackboxSettings.getEMdpNeighborhoodState(), blackboxSettings.getEMdpNeighborhoodDepth(), std::cout);
        } else {
            std::ofstream outFile(blackboxSettings.getEMdpNeighborhoodDotOutFileName());
            dotGenEMdp.convertNeighborhood(eMDPtemp, blackboxSettings.getEMdpNeighborhoodState(), blackboxSettings.getEMdpNeighborhoodDepth(), outFile);
            outFile.close();
        }
    }
}


template<typename StateType>
void executeBMdpFlags(settings::modules::BlackboxSettings blackboxSettings, BMdp<StateType> bMDP) {
    if(blackboxSettings.isSetPrintBMdp()) //Print the explored bMDP if flag is set 
        bMDP.printModelInformationToStream(std::cout);
    if(blackboxSettings.isSetBMdpToDot()) { //Convert bMDP to dot 
        if(blackboxSettings.getBMdpDotOutFileName() == "log") {
            bMDP.writeDotToStream(std::cout, 30, blackboxSettings.isSetDotIncLab(), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, true);
        } else {
            std::ofstream outFile(blackboxSettings.getBMdpDotOutFileName());
            bMDP.writeDotToStream(outFile, 30, blackboxSettings.isSetDotIncLab(), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, true);
            outFile.close();
        }
    }
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

    // init objects for algorithm
    EMdp<StateType> eMDP;

    auto heuristicSim(std::static_pointer_cast<heuristicSim::HeuristicSim<StateType, ValueType>>(std::make_shared<heuristicSim::NaiveHeuristicSim<StateType, ValueType>>(blackboxMDP, seedSimHeuristic)));
    BlackBoxExplorer<StateType, ValueType> blackboxExplorer(blackboxMDP, heuristicSim);
    auto boundFunc = getBoundFunc<ValueType>(boundFuncType);
    auto deltaDist = getDeltaDistribution<StateType>(deltaDistType);
    std::pair<double, double> valueBounds = std::make_pair(0, 1);

    // init objects for output generation

    // run 3 step algorithm
    uint_fast64_t iterCount = 0;
    while (eps < valueBounds.second - valueBounds.first && iterCount < maxIterations) {
        iterCount++;

        // simulate
        blackboxExplorer.performExploration(eMDP, simulationsPerIter);

        // infer 
        BMdp<ValueType> bMDP = infer<StateType, ValueType>(eMDP, *boundFunc, *deltaDist, blackboxMDP->getPmin(), delta, !blackboxMDP->isGreybox());

        if(iterCount == maxIterations) //cli argument execution for eMDP and bMDP 
            executeBMdpFlags(blackboxSettings, bMDP); 
            executeEMdpFlags(blackboxSettings, eMDP);
        
        // TODO create infer output
        // value approximation (implemented some time in future)
    }

    
    

    // TODO return actual result when it can be computed
    return  std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(0, 1);
}





template class BlackBoxChecker<storm::models::sparse::Mdp<double>, uint32_t>;
template class BlackBoxChecker<storm::models::sparse::Mdp<double>, uint64_t>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm

//TODO einzelnd kompilieren
//TODO über storm executeable ausführbar sein
