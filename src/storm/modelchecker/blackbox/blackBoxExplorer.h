//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_BLACKBOXEXPLORER_H
#define STORM_BLACKBOXEXPLORER_H

#include <random>

#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/modelchecker/exploration/Statistics.h"

#include "storm/modelchecker/blackbox/eMDP.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/heuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class StateType, class ValueType>
class blackBoxExplorer {
    public:
     typedef StateType ActionType;
     typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;

     blackBoxExplorer();

     void performExploration(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType>& stateGen,
                             storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo,
                             heuristic_simulate::heuristicSim& heuristic, int samplePathCount);

     //eMDP<ValueType>* getEMDP() {
     //   return &this->eMDP;
     //};

    private:
     void samplePathFromInitialState(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType>& stateGen,
                                     storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo,
                                     StateActionStack stack,
                                     heuristic_simulate::heuristicSim& heuristic,
                                     storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats);

     ActionType sampleActionOfState(StateType state,
                                    storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo,
                                    heuristic_simulate::heuristicSim& heuristic);
     
     StateType sampleSuccessorFromAction(StateType state,
                                         ActionType action,
                                         storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo);
     
     void addStateToEmdpIfNew(StateType state, storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo);

     bool exploreState(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType>& stateGen,
                       StateType state,
                       storm::generator::CompressedState const& compressedState,
                       storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo,
                       storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats);

     storm::modelchecker::blackbox::eMDP<int> eMDP;
     mutable std::default_random_engine randomGenerator;
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXEXPLORER_H
