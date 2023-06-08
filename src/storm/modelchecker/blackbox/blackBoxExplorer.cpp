//
// Created by Maximilian Kamps on 25.05.23.
//

#include "storm/modelchecker/blackbox/blackBoxExplorer.h"

#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/modelchecker/exploration/StateGeneration.h"

// maybe useful class
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class StateType, class ValueType>
blackBoxExplorer<StateType, ValueType>::blackBoxExplorer(): eMDP(){
    
}

template <class StateType, class ValueType>
void blackBoxExplorer<StateType, ValueType>::performExploration(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType> stateGen,
                                                                storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType> explInfo,
                                                                storm::modelchecker::blackbox::heuristic_simulate::heuristicSim& heuristic) {
}

template class blackBoxExplorer<uint32_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm