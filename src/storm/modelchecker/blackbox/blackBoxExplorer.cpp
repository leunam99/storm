//
// Created by Maximilian Kamps on 25.05.23.
//

#include "storm/modelchecker/blackbox/blackBoxExplorer.h"

#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/modelchecker/exploration/Statistics.h"

// maybe useful class
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class StateType, class ValueType>
blackBoxExplorer<StateType, ValueType>::blackBoxExplorer(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType>& stateGen,
                                                         storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo): 
                                                         eMDP(), stateGen(std::move(stateGen), explInfo(std::move(explInfo))){
    
}

template <class StateType, class ValueType>
void blackBoxExplorer<StateType, ValueType>::performExploration(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType> stateGen,
                                                                storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType> explInfo,
                                                                storm::modelchecker::blackbox::heuristic_simulate::heuristicSim& heuristic,
                                                                int samplePathCount) {
    stateGeneration.computeInitialStates();
    STORM_LOG_THROW(stateGeneration.getNumberOfInitialStates() == 1, storm::exceptions::NotSupportedException,
                    "Currently only models with one initial state are supported by the blackboxExplorer");
    StateType initialStateIndex = stateGeneration.getFirstInitialState();
    
    // Create a stack that is used to track the path we sampled.
    StateActionStack stack;
    storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats;

    for (int i = 0; i < samplePathCount; i++) {
        samplePathFromInitialState(stateGen explInfo, heuristic, stats);

        // store explorationSteps in eMDP
        for (auto& it = stack.begin(); it != stack.end(); it++) {
            StateType state = it.first();
            ActionType action = it.second();

            if (!eMDP.isStateKnown(state)) {
                //eMDP.addState(state, stateGen.);
            }
        }
    }
}

template <class StateType, class ValueType>
void blackBoxExplorer<StateType, ValueType>::samplePathFromInitialState(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType> stateGen,
                                                                        storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType> explInfo,
                                                                        heuristic_simulate::heuristicSim& heuristic,
                                                                        storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats) {

} 

template class blackBoxExplorer<uint32_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm