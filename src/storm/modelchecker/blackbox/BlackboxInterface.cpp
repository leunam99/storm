#include "storm/modelchecker/blackbox/BlackboxInterface.h"

#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/logic/Formula.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/utility/macros.h"

#include "storm/generator/CompressedState.h"
#include "storm/solver/OptimizationDirection.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BlackboxSettings.h"


namespace storm {
namespace modelchecker {
namespace blackbox {

template <typename StateType>
StateType BlackboxMDP<StateType>::getSucCount(StateType state, StateType action) {
    STORM_LOG_THROW(isGreybox(), storm::exceptions::NotImplementedException, "getSucCount is not implemented for this greybox MDP");
    STORM_LOG_THROW(!isGreybox(), storm::exceptions::NotSupportedException, "getSucCount is not implemented for this blackbox MDP");
    return 0;
}

template <typename StateType>
double BlackboxMDP<StateType>::getPmin() {
    return storm::settings::getModule<storm::settings::modules::BlackboxSettings>().getPMin();
}

template <typename StateType, typename ValueType>
BlackboxWrapperOnWhitebox<StateType, ValueType>::BlackboxWrapperOnWhitebox(storm::prism::Program const& program)
                                                : program(program.substituteConstantsFormulas()),
                                                  explorationInformation(storm::OptimizationDirection::Maximize),
                                                  stateGenerationLabels(this->program, explorationInformation, 
                                                  this->program.getManager().boolean(true), this->program.getManager().boolean(true)) {
    // compute and explore initial state
    explorationInformation.newRowGroup(0);
    stateGenerationLabels.computeInitialStates();
    exploreState(stateGenerationLabels.getFirstInitialState());
    stateMappingInOut[stateGenerationLabels.getFirstInitialState()] = 0;
    stateMappingOutIn[0] = getInitialState();
}

template <typename StateType, typename ValueType>
StateType BlackboxWrapperOnWhitebox<StateType, ValueType>::getInitialState() {
    return stateMappingInOut.at(stateGenerationLabels.getFirstInitialState());
}

template <typename StateType, typename ValueType>
StateType BlackboxWrapperOnWhitebox<StateType, ValueType>::getAvailActions(StateType state) {
    StateType stateIdx = stateMappingOutIn.at(state);
    StateType stateRow = explorationInformation.getRowGroup(stateIdx);
    return explorationInformation.getRowGroupSize(stateRow);
}

template <typename StateType, typename ValueType>
StateType BlackboxWrapperOnWhitebox<StateType, ValueType>::sampleSucc(StateType state, StateType action) {
    StateType stateIdx = stateMappingOutIn.at(state);
    StateType stateRowIdx = explorationInformation.getStartRowOfGroup(explorationInformation.getRowGroup(stateIdx));
    auto& actionRow = explorationInformation.getRowOfMatrix(stateRowIdx + action);

    std::uniform_int_distribution<StateType> distribution(0, actionRow.size() - 1);
    StateType successor = actionRow[distribution(randomGenerator)].getColumn();
    
    // explore successor and add new unexplored states if necessary
    if (explorationInformation.isUnexplored(successor)) {
        exploreState(successor);
    }
    // if successor is returned the first time, add mapping
    if (stateMappingInOut.find(successor) == stateMappingInOut.end()) {
        stateMappingInOut[successor] = stateMappingInOut.size();
        stateMappingOutIn[stateMappingInOut.at(successor)] = successor;
    }
    return stateMappingInOut.at(successor);
}

template <typename StateType, typename ValueType>
bool BlackboxWrapperOnWhitebox<StateType, ValueType>::isGreybox() {
    return false;
}

template <typename StateType, typename ValueType>
std::set<std::string> BlackboxWrapperOnWhitebox<StateType, ValueType>::getStateLabels(StateType state) {
    StateType stateIdx = stateMappingOutIn[state];
    //only update stateLabelin if necessary
    if (stateIdx >= stateLabeling.getNumberOfItems()) {
        stateLabeling = stateGenerationLabels.label();
    }
    return stateLabeling.getLabelsOfState(stateIdx);
}

template <typename StateType, typename ValueType>
void BlackboxWrapperOnWhitebox<StateType, ValueType>::exploreState(StateType stateIdx) {
    // This is a private function, it accepts stateIdx to be an internal index

    auto unexploredIt = explorationInformation.findUnexploredState(stateIdx);
    if (unexploredIt == explorationInformation.unexploredStatesEnd()) {
        return;
    }

    explorationInformation.assignStateToNextRowGroup(stateIdx);
    storm::generator::CompressedState comprState = unexploredIt->second;

    // get actions; store them and their successors in explorationInformation
    stateGenerationLabels.load(comprState);
    storm::generator::StateBehavior<ValueType, exploration_state_type> behavior = stateGenerationLabels.expand();    
    
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
    explorationInformation.removeUnexploredState(unexploredIt);
}

template class BlackboxMDP<uint32_t>;
template class BlackboxMDP<uint64_t>;

template class BlackboxWrapperOnWhitebox<uint32_t, double>;
template class BlackboxWrapperOnWhitebox<uint64_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
