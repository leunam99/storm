#include "storm/modelchecker/blackbox/blackbox_interface.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/utility/macros.h"

#include "storm/generator/CompressedState.h"


namespace storm {
namespace modelchecker {
namespace blackbox {

typename blackboxMDP::index_type blackboxMDP::get_suc_count(blackboxMDP::index_type state, blackboxMDP::index_type action) {
    STORM_LOG_THROW(is_greybox(), storm::exceptions::NotImplementedException, "get_suc_count is not implemented for this greybox MDP");
    STORM_LOG_THROW(!is_greybox(), storm::exceptions::NotSupportedException, "get_suc_count is not implemented for this greybox MDP");
}

double blackboxMDP::get_pmin() {
    throw storm::exceptions::NotImplementedException();
}

template <typename StateType, typename ValueType>
blackboxWrapperOnWhitebox<StateType, ValueType>::blackboxWrapperOnWhitebox(storm::prism::Program& program)
                                                : explorationInformation(storm::OptimizationDirection::Maximize),
                                                  stateGeneration(program, explorationInformation, 
                                                  (storm::expressions::Expression()), (storm::expressions::Expression())) {
    // intentionally left empty
}

template <typename StateType, typename ValueType>
typename blackboxWrapperOnWhitebox<StateType, ValueType>::index_type blackboxWrapperOnWhitebox<StateType, ValueType>::get_initial_state() {
    stateGeneration.computeInitialStates();
    return stateGeneration.getFirstInitialState();
}

template <typename StateType, typename ValueType>
typename blackboxWrapperOnWhitebox<StateType, ValueType>::index_type blackboxWrapperOnWhitebox<StateType, ValueType>::get_avail_actions(index_type state) {
    StateType stateRow = explorationInformation.getRowGroup(state);
    return explorationInformation.getRowGroupSize(stateRow);
}

template <typename StateType, typename ValueType>
typename blackboxWrapperOnWhitebox<StateType, ValueType>::index_type blackboxWrapperOnWhitebox<StateType, ValueType>::sample_suc(index_type state, index_type action) {
    StateType stateRowIdx = explorationInformation.getStartRowOfGroup(explorationInformation.getRowGroup(state));
    auto& actionRow = explInfo.getRowOfMatrix(stateRowIdx + action);

    std::uniform_int_distribution<ActionType> distribution(0, actionRow.size() - 1);
    StateType successor = actionRow[distribution(randomGenerator)].getColumn();
    
    // explore successor and add new unexplored states if necessary
    if (explorationInformation.isUnexplored(successor)) {
        exploreState(successor);
    }
    return successor;
}

template <typename StateType, typename ValueType>
bool blackboxWrapperOnWhitebox<StateType, ValueType>::is_greybox() {
    return false;
}

template <typename StateType, typename ValueType>
void blackboxWrapperOnWhitebox<StateType, ValueType>::exploreState(index_type state) {
    // TODO optimization: terminal and target states don't need to explored

    auto& unexploredIt = explorationInformation.findUnexploredState(currentStateId);
    if (unexploredIt == explorationInformation.unexploredStatesEnd()) {
        return;
    }

    storm::generator::CompressedState comprState = unexploredIt->second;

    // get actions; store them and their successors in explorationInformation
    stateGeneration.load(comprState);
    storm::generator::StateBehavior<ValueType, StateType> behavior = stateGeneration.expand();    
    
    StateType startAction = explorationInformation.getActionCount();
    explorationInformation.addActionsToMatrix(behavior.getNumberOfChoices());
    StateType localAction = 0;
    
    for (auto const& choice : behavior) {
        for (auto const& entry : choice) {
           explorationInformation.getRowOfMatrix(startAction + localAction).emplace_back(entry.first, entry.second);
        }
        ++localAction;
    }

    explorationInformation.terminateCurrentRowGroup();
    explorationInformation.removeUnexploredState(successor);
}


} //namespace blackbox
} //namespace modelchecker
} //namespace storm
