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
                                     heuristic_simulate::heuristicSim& heuristic,
                                     storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats);

     StateType getAvailActionsCount(StateType state);
     ActionType sampleActionOfState(StateType state, heuristic_simulate::heuristicSim& heuristic);
     void addStateToEmdpIfNew(StateType state, storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo);

     storm::modelchecker::blackbox::eMDP<int> eMDP;
     //mutable std::default_random_engine randomGenerator;
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXEXPLORER_H
