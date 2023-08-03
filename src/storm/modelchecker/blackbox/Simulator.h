#pragma once

#include "storm/modelchecker/blackbox/BlackboxInterface.h"
#include "storm/modelchecker/blackbox/EMdp.h"
#include "storm/modelchecker/blackbox/heuristicSimulate/HeuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename StateType, typename ValueType>
class Simulator {
    typedef StateType ActionType;
    typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;

   public:
    /*!
     * returns a Simulator for the given blackbox_interface and simulation heuristic
     *
     * @param blackboxMDP
     * @param heuristicSim
     * @return Simulator
     */
    Simulator(std::shared_ptr<BlackboxMDP<StateType, ValueType>> blackboxMDP, std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim);

    /*!
     * simulate paths in the blackboxMDP and stores the visits in the given eMDP
     *
     * @param eMDP
     * @param numExploration number of paths to explore
     */
    void simulate(EMdp<StateType>& eMDP, StateType numExplorations);

   private:
    std::shared_ptr<BlackboxMDP<StateType, ValueType>> blackboxMdp;
    std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim;
};

}  // namespace blackbox
}  // namespace modelchecker
}  // namespace storm
