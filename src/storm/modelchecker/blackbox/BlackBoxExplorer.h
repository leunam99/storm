//
// Created by Maximilian Kamps on 25.05.23.
//

#include "storm/modelchecker/blackbox/BlackboxInterface.h"
#include "storm/modelchecker/blackbox/EMdp.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"

#ifndef STORM_BLACKBOXEXPLORER_H
#define STORM_BLACKBOXEXPLORER_H

namespace storm {
namespace modelchecker {
namespace blackbox {

template <typename StateType, typename ValueType>
class BlackBoxExplorer {
    typedef StateType ActionType;
    typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;
 
    public:
     /*!
      * returns a BlackBoxExplorer for the given blackbox_interface and simulation heuristic
      * 
      * @param blackboxMDP 
      * @param heuristicSim
      * @return BlackBoxExplorer
      */
     BlackBoxExplorer(std::shared_ptr<BlackboxMDP<StateType, ValueType>> blackboxMDP, std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim);

     /*!
      * explores paths in the blackboxMDP and stores the visits in the given eMDP
      * 
      * @param eMDP 
      * @param numExploration number of paths to explore
      */
     void performExploration(EMdp<StateType>& eMDP, StateType numExplorations);

    private:
     std::shared_ptr<BlackboxMDP<StateType, ValueType>> blackboxMdp;
     std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim;

};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXEXPLORER_H
