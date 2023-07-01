//
// Created by Maximilian Kamps on 25.05.23.
//

#include "storm/modelchecker/blackbox/blackbox_interface.h"
#include "storm/modelchecker/blackbox/eMDP.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/heuristicSim.h"

#ifndef STORM_BLACKBOXEXPLORER_H
#define STORM_BLACKBOXEXPLORER_H

namespace storm {
namespace modelchecker {
namespace blackbox {

template <typename StateType, typename ValueType>
class BlackBoxExplorer {
    typedef StateType ActionType;
    typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;
    typedef int_fast32_t index_type_fast;
 
    public:
     /*!
      * returns a BlackBoxExplorer for the given blackbox_interface and simulation heuristic
      * 
      * @param blackboxMDP 
      * @param heuristicSim
      * @return BlackBoxExplorer
      */
     BlackBoxExplorer(std::shared_ptr<BlackboxMDP<StateType>> blackboxMDP, std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim);

     /*!
      * explores paths in the blackboxMDP and stores the visits in the given eMDP
      * 
      * @param eMDP 
      * @param numExploration number of paths to explore
      */
     void performExploration(EMdp<index_type_fast>& eMDP, StateType numExplorations);

    private:
     std::shared_ptr<BlackboxMDP<StateType>> blackboxMdp;
     std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim;

};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXEXPLORER_H
