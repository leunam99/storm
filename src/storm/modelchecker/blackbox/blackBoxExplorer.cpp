//
// Created by Maximilian Kamps on 25.05.23.
//

#include "storm/modelchecker/blackbox/blackBoxExplorer.h"

#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/modelchecker/exploration/Statistics.h"

#include "storm/modelchecker/blackbox/eMDP.h"
#include "storm/solver/OptimizationDirection.h"


// maybe useful class
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class StateType, class ValueType>
blackBoxExplorer<StateType, ValueType>::blackBoxExplorer(): eMDP() {
    
}

template <class StateType, class ValueType>
void blackBoxExplorer<StateType, ValueType>::performExploration(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType>& stateGen,
                                                                storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo,
                                                                storm::modelchecker::blackbox::heuristic_simulate::heuristicSim& heuristic,
                                                                int samplePathCount) {
    stateGen.computeInitialStates();
    //STORM_LOG_THROW(stateGen.getNumberOfInitialStates() == 1, storm::exceptions::NotSupportedException,
    //                "Currently only models with one initial state are supported by the blackboxExplorer");
    StateType initialStateIndex = stateGen.getFirstInitialState();
    
    // Create a stack that is used to track the path we sampled.
    StateActionStack stack;
    storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats;

    for (int i = 0; i < samplePathCount; i++) {
        samplePathFromInitialState(stateGen, explInfo, stack, heuristic, stats);

        // make sure sampled path is long enough
        if (stack.size() < 2) {
            continue;
        }

        StateType state = stack[0].first;
        ActionType action = stack[0].second;
        StatType succ;
        // store explorationSteps in eMDP
        for (auto& it = std::next(stack.begin()); it != stack.end(); it++) {
            succ = it.first;

            addStateToEmdpIfNew(state, explInfo);
            eMDP.addVisit(state, action, succ);

            state = it.first;
            action = it.second;
        }
    }
}

template <class StateType, class ValueType>
void blackBoxExplorer<StateType, ValueType>::samplePathFromInitialState(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType>& stateGen,
                                                                        storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo,
                                                                        heuristic_simulate::heuristicSim& heuristic,
                                                                        storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats) {

} 

template<class StateType, class ValueType>
void blackBoxExplorer<StateType, ValueType>::addStateToEmdpIfNew(StateType state,
                                                                 storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo) {
    if (eMDP.isStateKnown(state)) {
        return;
    }
    // get availActionCount
    StateType stateRow = explInfo.getRowGroup(state);
    ActionType availActions = explInfo.getRowGroupSize(stateRow);
    std::vector<typename storm::modelchecker::blackbox::eMDP::index_type> actionsVec(availActions);
    std::iota(actionsVec.begin(), actionsVec.end(), 0);
    eMDP.addState(state, actionsVec);
}


template class blackBoxExplorer<uint32_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm