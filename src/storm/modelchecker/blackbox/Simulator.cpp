//
// Created by Maximilian Kamps on 25.05.23.
//

#include "storm/modelchecker/blackbox/Simulator.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"
#include "storm/modelchecker/blackbox/EMdp.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template <typename StateType, typename ValueType>
Simulator<StateType, ValueType>::Simulator(std::shared_ptr<BlackboxMDP<StateType, ValueType>> blackboxMDP, std::shared_ptr<heuristicSim::HeuristicSim<StateType, ValueType>> heuristicSim) :
                                                         blackboxMdp(blackboxMDP), heuristicSim(heuristicSim) {
    // intentionally empty
}

template <typename StateType, typename ValueType>
void Simulator<StateType, ValueType>::simulate(EMdp<StateType>& emdp, StateType numExplorations) {
    StateActionStack stack;
    StateType maxPathLen = 10; // TODO magicNumber, collect constants

    // set initial state
    emdp.addInitialState(blackboxMdp->getInitialState());
    // safe highest stateIdx, to later update new states
    StateType latestExploredState = emdp.getTotalStateCount() - 1;

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

        // save stack in emdp
        StateType state;
        suc = stack.back().first;
        stack.pop_back();
        while (!stack.empty()) {
            state = stack.back().first;
            if (!emdp.isStateKnown(state)) {
                // init State with all actions in emdp
                for (StateType action = 0; action < blackboxMdp->getAvailActions(state); action++) {
                    emdp.addUnsampledAction(state, action);

                    if(blackboxMdp->isGreybox()){
                        emdp.setSuccCount(state, action, blackboxMdp->getSucCount(state, action));
                    }
                }

                // TODO add Reward
            }

            actionTaken = stack.back().second;
            emdp.addVisit(state, actionTaken, suc);
            suc = state;
            stack.pop_back();
        }

        // update maxPathLen
        maxPathLen = 3 * emdp.getTotalStateCount();  // TODO magic number; collect constants
    }

    // set labels of new states
    for (StateType i = latestExploredState; i < emdp.getTotalStateCount(); i++) {
        for (auto const &label: blackboxMdp->getStateLabels(i))
        emdp.addStateLabel(label, i);
        // action labels
        for (StateType a = 0; a < blackboxMdp->getAvailActions(i); a++) {
            for (auto& label: blackboxMdp->getActionLabels(i, a)) {
                emdp.addActionLabel(label, i, a);
            }
        }
    }
}

template class Simulator<uint32_t, double>;
template class Simulator<uint64_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
