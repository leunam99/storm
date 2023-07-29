//
// Created by Maximilian Kamps on 25.05.23.
//

#include "BlackBoxExplorer.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"
#include "storm/modelchecker/blackbox/EMdp.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/HeuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template <typename StateType, typename ValueType>
BlackBoxExplorer<StateType, ValueType>::BlackBoxExplorer(std::shared_ptr<BlackboxMDP<StateType, ValueType>> blackboxMDP, std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim) :
                                                         blackboxMdp(blackboxMDP), heuristicSim(heuristicSim) {
    // intentionally empty
}

template <typename StateType, typename ValueType>
void BlackBoxExplorer<StateType, ValueType>::performExploration(EMdp<StateType>& eMDP, StateType numExplorations) {
    StateActionStack stack;
    StateType maxPathLen = 10; // TODO magicNumber, collect constants

    // set initial state
    eMDP.addInitialState(blackboxMdp->getInitialState());
    // safe highest stateIdx, to later update new states
    StateType latestExploredState = eMDP.getTotalStateCount() - 1;

    for (StateType i = 0; i < numExplorations; i++) {
        stack.push_back(std::make_pair(blackboxMdp->getInitialState(), 0));
        ActionType actionTaken;
        StateType suc;
        // do exploration
        while (stack.size() < maxPathLen && !heuristicSim->shouldStopSim(stack)) {
            actionTaken = heuristicSim->sampleAction(stack);
            suc = blackboxMdp->sampleSucc((stack.back().first), actionTaken);

            // save in stack
            stack.back().second = actionTaken;
            stack.push_back(std::make_pair(suc, 0));
        }

        // save stack in eMDP
        StateType state;
        suc = stack.back().first;
        stack.pop_back();
        while (!stack.empty()) {
            state = stack.back().first;
            if (!eMDP.isStateKnown(state)) {
                // init State with all actions in eMDP
                for (StateType action = 0; action < blackboxMdp->getAvailActions(state); action++) {
                    eMDP.addUnsampledAction(state, action);

                    if(blackboxMdp->isGreybox()){
                        eMDP.setSuccCount(state, action, blackboxMdp->getSucCount(state, action));
                    }
                }

                // TODO add Reward
            }

            actionTaken = stack.back().second;
            eMDP.addVisit(state, actionTaken, suc);
            suc = state;
            stack.pop_back();
        }

        // update maxPathLen
        maxPathLen = 3 * eMDP.getTotalStateCount();  // TODO magic number; collect constants
    }

    // set labels of new states
    for (StateType i = latestExploredState; i < eMDP.getTotalStateCount(); i++) {
        for (auto const &label: blackboxMdp->getStateLabels(i))
        eMDP.addStateLabel(label, i);
        // action labels
        for (StateType a = 0; a < blackboxMdp->getAvailActions(i); a++) {
            for (auto& label: blackboxMdp->getActionLabels(i, a)) {
                eMDP.addActionLabel(label, i, a);
            }
        }
    }
}

template class BlackBoxExplorer<uint32_t, double>;
template class BlackBoxExplorer<uint64_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
