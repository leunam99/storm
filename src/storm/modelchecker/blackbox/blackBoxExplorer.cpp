//
// Created by Maximilian Kamps on 25.05.23.
//
#include <iterator>

#include "storm/modelchecker/blackbox/blackBoxExplorer.h"

#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/modelchecker/exploration/Statistics.h"

#include "storm/modelchecker/blackbox/eMDP.h"
#include "storm/solver/OptimizationDirection.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidTypeException.h"

// maybe useful class
#include "storm/settings/modules/ModuleSettings.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<class StateType, class ValueType>
blackBoxExplorer<StateType, ValueType>::blackBoxExplorer(): eMdp() {  
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
        std::cout << "stack size before sampling: "<< stack.size() << "\n";

        samplePathFromInitialState(stateGen, explInfo, stack, heuristic, stats);

        // make sure sampled path is long enough
        std::cout << "stack size: "<< stack.size() << "\n";
        if (stack.size() < 2) {
            continue;
        }

        StateType state;
        ActionType action;
        StateType succ = stack.back().first;

        stack.pop_back();
        while (!stack.empty()) {
            state = stack.back().first;
            action = stack.back().second;

            addStateToEmdpIfNew(state, explInfo);
            std::cout << "add to eMDP " << state << ", " << action << ", " << succ << "\n";
            eMdp.addVisit(state, action, succ);
            
            succ = state;
            stack.pop_back();
        }
    }
}

template <class StateType, class ValueType>
eMDP<typename blackBoxExplorer<StateType, ValueType>::eMdpType>* blackBoxExplorer<StateType, ValueType>::getEmdp() {
    return &eMdp;
}

template <class StateType, class ValueType>
void blackBoxExplorer<StateType, ValueType>::samplePathFromInitialState(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType>& stateGeneration,
                                                                        storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explorationInformation,
                                                                        StateActionStack& stack,
                                                                        heuristic_simulate::heuristicSim& heuristic,
                                                                        storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats) {
    // This function is mainly copied from SparseExplorationModelChecker, however parts regarding the precomputation are omitted

    // Start the search from the initial state.
    stack.push_back(std::make_pair(stateGeneration.getFirstInitialState(), 0));

    // As long as we didn't find a terminal (accepting or rejecting) state in the search, sample a new successor.
    bool foundTerminalState = false;
    while (!foundTerminalState) {
        StateType const& currentStateId = stack.back().first;
        
        auto unexploredIt = explorationInformation.findUnexploredState(currentStateId);
        if (unexploredIt != explorationInformation.unexploredStatesEnd()) {
            STORM_LOG_TRACE("State was not yet explored.");

            // Explore the previously unexplored state.
            storm::generator::CompressedState const& compressedState = unexploredIt->second;
            foundTerminalState = exploreState(stateGeneration, currentStateId, compressedState, explorationInformation, stats);
            if (foundTerminalState) {
                STORM_LOG_TRACE("Aborting sampling of path, because a terminal state was reached.");
            }
            explorationInformation.removeUnexploredState(unexploredIt);
        } else {
            // If the state was already explored, we check whether it is a terminal state or not.
            if (explorationInformation.isTerminal(currentStateId)) {
                STORM_LOG_TRACE("Found already explored terminal state: " << currentStateId << ".");
                foundTerminalState = true;
            }
        }

        // Notify the stats about the performed exploration step.
        stats.explorationStep();

        // If the state was not a terminal state, we continue the path search and sample the next state.
        if (!foundTerminalState) {
            // At this point, we can be sure that the state was expanded and that we can sample according to the
            // probabilities in the matrix.
            StateType chosenAction = sampleActionOfState(currentStateId, explorationInformation, heuristic);
            stack.back().second = chosenAction;
            STORM_LOG_TRACE("Sampled action " << chosenAction << " in state " << currentStateId << ".");

            StateType successor = sampleSuccessorFromAction(currentStateId, chosenAction, explorationInformation);
            STORM_LOG_TRACE("Sampled successor " << successor << " according to action " << chosenAction << " of state " << currentStateId << ".");

            // Put the successor state and a dummy action on top of the stack.
            stack.emplace_back(successor, 0);

            // If the number of exploration steps exceeds a certain threshold (3 times explored states count), break
            if (stack.size() > 3 * explorationInformation.getActionCount()) {
                break;
            }
        }
    }
} 

template<class StateType, class ValueType>
typename blackBoxExplorer<StateType, ValueType>::ActionType blackBoxExplorer<StateType, ValueType>::sampleActionOfState(StateType state,
                                                                                                                        storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo,
                                                                                                                        heuristic_simulate::heuristicSim& heuristic) {
    StateType stateRow = explInfo.getRowGroup(state);
    ActionType availActions = explInfo.getRowGroupSize(stateRow);

    // for now just choose action by uniform distribution
    if (heuristic.getType() == heuristic_simulate::HeuristicsSim::NAIVE) {
        std::uniform_int_distribution<ActionType> distribution(0, availActions - 1);
        return distribution(randomGenerator);
    } else {
        STORM_LOG_THROW(true, storm::exceptions::InvalidTypeException, "Only NAIVE heuristic is implemented right now.");
    }
    return 0;
}


template<class StateType, class ValueType>
StateType blackBoxExplorer<StateType, ValueType>::sampleSuccessorFromAction(StateType state,
                                                                            ActionType action,
                                                                            storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo) {
    StateType stateRow = explInfo.getRowGroup(state);
    auto& actionRow = explInfo.getRowOfMatrix(stateRow + action);
    std::uniform_int_distribution<ActionType> distribution(0, actionRow.size() - 1);
    return actionRow[distribution(randomGenerator)].getColumn();
}


template<class StateType, class ValueType>
void blackBoxExplorer<StateType, ValueType>::addStateToEmdpIfNew(StateType state,
                                                                 storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explInfo) {
    if (eMdp.isStateKnown(state)) {
        return;
    }
    // get availActionCount
    StateType stateRow = explInfo.getRowGroup(state);
    ActionType availActions = explInfo.getRowGroupSize(stateRow);
    std::vector<typename storm::modelchecker::blackbox::eMDP<ValueType>::index_type> actionsVec(availActions);
    std::iota(actionsVec.begin(), actionsVec.end(), 0);
    eMdp.addState(state, actionsVec);
}


template<class StateType, class ValueType>
bool blackBoxExplorer<StateType, ValueType>::exploreState(storm::modelchecker::exploration_detail::StateGeneration<StateType, ValueType>& stateGeneration,
                                                          StateType state,
                                                          storm::generator::CompressedState const& compressedState,
                                                          storm::modelchecker::exploration_detail::ExplorationInformation<StateType, ValueType>& explorationInformation,
                                                          storm::modelchecker::exploration_detail::Statistics<StateType, ValueType> stats) {
    // mainly copied from SparseExplorationModelChecker::exploreState
    
    bool isTerminalState = false;
    bool isTargetState = false;

    ++stats.numberOfExploredStates;

    // Finally, map the unexplored state to the row group.
    explorationInformation.assignStateToNextRowGroup(state);
    STORM_LOG_TRACE("Assigning row group " << explorationInformation.getRowGroup(state) << " to state " << state << ".");

    // Before generating the behavior of the state, we need to determine whether it's a target state that
    // does not need to be expanded.
    stateGeneration.load(compressedState);
    if (stateGeneration.isTargetState()) {
        ++stats.numberOfTargetStates;
        isTargetState = true;
        isTerminalState = true;
    } else if (stateGeneration.isConditionState()) {
        STORM_LOG_TRACE("Exploring state.");

        // If it needs to be expanded, we use the generator to retrieve the behavior of the new state.
        storm::generator::StateBehavior<ValueType, StateType> behavior = stateGeneration.expand();
        STORM_LOG_TRACE("State has " << behavior.getNumberOfChoices() << " choices.");

        // Clumsily check whether we have found a state that forms a trivial BMEC.
        bool otherSuccessor = false;
        for (auto const& choice : behavior) {
            for (auto const& entry : choice) {
                if (entry.first != state) {
                    otherSuccessor = true;
                    break;
                }
            }
        }
        isTerminalState = !otherSuccessor;

        // If the state was neither a trivial (non-accepting) terminal state nor a target state, we
        // need to store its behavior.
        if (!isTerminalState) {
            // Next, we insert the behavior into our matrix structure.
            StateType startAction = explorationInformation.getActionCount();
            explorationInformation.addActionsToMatrix(behavior.getNumberOfChoices());

            ActionType localAction = 0;

           for (auto const& choice : behavior) {
                for (auto const& entry : choice) {
                    explorationInformation.getRowOfMatrix(startAction + localAction).emplace_back(entry.first, entry.second);
                    STORM_LOG_TRACE("Found transition " << state << "-[" << (startAction + localAction) << ", " << entry.second << "]-> "
                                                        << entry.first << ".");
                }

               ++localAction;
            }

            // Terminate the row group.
            explorationInformation.terminateCurrentRowGroup();

        }
    } else {
        // In this case, the state is neither a target state nor a condition state and therefore a rejecting
        // terminal state.
        isTerminalState = true;
    }

    if (isTerminalState) {
        STORM_LOG_TRACE("State does not need to be explored, because it is " << (isTargetState ? "a target state" : "a rejecting terminal state") << ".");
        explorationInformation.addTerminalState(state);

        // Increase the size of the matrix, but leave the row empty.
        explorationInformation.addActionsToMatrix(1);

        // Terminate the row group.
        explorationInformation.newRowGroup();
    }

    return isTerminalState;}

//template class blackBoxExplorer<uint32_t, double>;
template class blackBoxExplorer<uint32_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm