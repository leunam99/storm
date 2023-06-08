//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_BLACKBOXEXPLORER_H
#define STORM_BLACKBOXEXPLORER_H

#include <random>

#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/modelchecker/exploration/StateGeneration.h"

#include "storm/modelchecker/blackbox/eMDP.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/heuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class StateType, class ValueType>
class blackBoxExplorer {
    public:
     blackBoxExplorer();

     void performExploration(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType> stateGen,
                             storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType> explInfo,
                             heuristic_simulate::heuristicSim& heuristic);

     //eMDP<ValueType>* getEMDP() {
     //   return &this->eMDP;
     //};

    private:
     storm::modelchecker::blackbox::eMDP<int> eMDP;
     //mutable std::default_random_engine randomGenerator;
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXEXPLORER_H
