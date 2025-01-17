//
// Created by Maximilian Kamps on 25.05.23.
//
#include <fstream>
#include <string>

#include "storm/modelchecker/blackbox/BlackboxChecker.h"

#include "storm/modelchecker/blackbox/BMdp.h"
#include "storm/modelchecker/blackbox/Simulator.h"
#include "storm/modelchecker/blackbox/EMdpToDot.h"
#include "storm/modelchecker/blackbox/boundFunctions/BoundFunc.h"
#include "storm/modelchecker/blackbox/deltaDistribution/DeltaDistribution.h"
#include "storm/modelchecker/blackbox/heuristicSimulate/HeuristicSim.h"
#include "storm/modelchecker/blackbox/Infer.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BlackboxSettings.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/macros.h"


namespace storm {
namespace modelchecker {
namespace blackbox {



template<typename ModelType, typename StateType>
BlackboxChecker<ModelType, StateType>::BlackboxChecker(storm::prism::Program const& program) {
    if(storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getIsGreybox()){
        auto ptr = std::make_shared<GreyboxWrapperOnWhitebox<StateType, ValueType>>(program);
        blackboxMDP = std::static_pointer_cast<BlackboxMDP<StateType, ValueType>>(ptr);
    } else {
        auto ptr = std::make_shared<BlackboxWrapperOnWhitebox<StateType, ValueType>>(program);
        blackboxMDP = std::static_pointer_cast<BlackboxMDP<StateType, ValueType>>(ptr);
    }
}

template<typename ModelType, typename StateType>
bool BlackboxChecker<ModelType, StateType>::canHandle(CheckTask<storm::logic::Formula, ValueType> const&) const {
    // TODO implement actual check
    return true;
}

template<typename StateType>
void executeEMdpFlags(settings::modules::BlackboxSettings blackboxSettings, EMdp<StateType> emdp) {
    if(blackboxSettings.isSetPrintEMdp()) //Print the explored emdp if flag is set 
        emdp.print();


    if(blackboxSettings.isSetWriteEMdpToFile()) { //Write to file if flag is set 
        emdp.emdpToFile(blackboxSettings.getEMdpOutFileName());
    }

    auto dotGenEMdp = EMdpDotGenerator<StateType>(blackboxSettings.isSetDotIncAct(), 
                                                  blackboxSettings.isSetDotIncSmpl(), 
                                                  blackboxSettings.isSetDotIncLab(), 
                                                  blackboxSettings.isSetDotIncCol());

    if(blackboxSettings.isSetEMdptoDot()) { //Dot coversion if flag is set 
        auto eMDPtemp = (blackboxSettings.getEMdpDotInFileName() == "expl") ? emdp : emdp.emdpFromFile(blackboxSettings.getEMdpDotInFileName());
        if(blackboxSettings.getEMdpDotOutFileName() == "log") {
            dotGenEMdp.convert(eMDPtemp, std::cout);
        } else {
            std::ofstream outFile(blackboxSettings.getEMdpDotOutFileName());
            dotGenEMdp.convert(eMDPtemp, outFile);
            outFile.close();
        }
    }

    if(blackboxSettings.isSetEMdpNeighbToDot()) { //Neighborhood Dot coversion if flag is set 
        auto eMDPtemp = emdp.emdpFromFile(blackboxSettings.getEMdpNeighborhoodDotInFileName());
        eMDPtemp.createReverseMapping();
        if(blackboxSettings.getEMdpNeighborhoodDotOutFileName() == "log") {
            dotGenEMdp.convertNeighborhood(eMDPtemp, blackboxSettings.getEMdpNeighborhoodState(), blackboxSettings.getEMdpNeighborhoodDepth(), std::cout);
        } else {
            std::ofstream outFile(blackboxSettings.getEMdpNeighborhoodDotOutFileName());
            dotGenEMdp.convertNeighborhood(eMDPtemp, blackboxSettings.getEMdpNeighborhoodState(), blackboxSettings.getEMdpNeighborhoodDepth(), outFile);
            outFile.close();
        }
    }
}


template<typename StateType>
void executeBMdpFlags(const settings::modules::BlackboxSettings& blackboxSettings, BMdp<StateType> bmdp) {
    if(blackboxSettings.isSetBMdpToDot()) { //Convert bmdp to dot 
        if(blackboxSettings.getBMdpDotOutFileName() == "log") {
            bmdp.writeDotToStream(std::cout, 30, blackboxSettings.isSetDotIncLab(), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, true);
        } else {
            std::ofstream outFile(blackboxSettings.getBMdpDotOutFileName());
            bmdp.writeDotToStream(outFile, 30, blackboxSettings.isSetDotIncLab(), nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, true);
            outFile.close();
        }
    }
}

template<typename ModelType, typename StateType>
std::unique_ptr<CheckResult> BlackboxChecker<ModelType, StateType>::computeUntilProbabilities(Environment const&, CheckTask<storm::logic::UntilFormula, ValueType> const&) {
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
    EMdp<StateType> emdp;

    auto heuristicSim(std::static_pointer_cast<heuristicSim::HeuristicSim<StateType, ValueType>>(std::make_shared<heuristicSim::NaiveHeuristicSim<StateType, ValueType>>(blackboxMDP, seedSimHeuristic)));
    Simulator<StateType, ValueType> simulator(blackboxMDP, heuristicSim);
    auto boundFunc = getBoundFunc<ValueType>(boundFuncType);
    auto deltaDist = getDeltaDistribution<StateType>(deltaDistType);
    std::pair<double, double> valueBounds = std::make_pair(0, 1);

    // run 3 step algorithm
    STORM_LOG_TRACE("Start SMC-Algorithm for blackbox MDP");
    uint_fast64_t iterCount = 0;
    while (eps < valueBounds.second - valueBounds.first && iterCount < maxIterations) {
        iterCount++;
        STORM_LOG_TRACE("Iteration " << iterCount << "/" << maxIterations);

        // simulate
        STORM_LOG_TRACE("Start SIMULATE phase");
        simulator.simulate(emdp, simulationsPerIter);

        // infer 
        auto bmdp = infer(emdp, *boundFunc, *deltaDist, blackboxMDP->getPmin(), delta, blackboxMDP->isGreybox(), blackboxMDP);

        if(iterCount == maxIterations) //cli argument execution for emdp and bmdp
            executeBMdpFlags(blackboxSettings, bmdp); 
        
        // value approximation (implemented some time in future)
    }
    executeEMdpFlags(blackboxSettings, emdp);

    // TODO return actual result when it can be computed
    return  std::make_unique<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>>(0, 1);
}





template class BlackboxChecker<storm::models::sparse::Mdp<double>, uint32_t>;
template class BlackboxChecker<storm::models::sparse::Mdp<double>, uint64_t>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
