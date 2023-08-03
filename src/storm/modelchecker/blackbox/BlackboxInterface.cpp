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

template <typename StateType, typename ValueType>
StateType BlackboxMDP<StateType, ValueType>::getSucCount(StateType, StateType) {
    STORM_LOG_THROW(isGreybox(), storm::exceptions::NotImplementedException, "getSucCount is not implemented for this greybox MDP");
    STORM_LOG_THROW(!isGreybox(), storm::exceptions::NotSupportedException, "getSucCount is not implemented for this blackbox MDP");
    return 0;
}

template <typename StateType, typename ValueType> BlackboxMDP<StateType, ValueType>::~BlackboxMDP() = default;

template <typename StateType, typename ValueType>
double BlackboxMDP<StateType, ValueType>::getPmin() {
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

    // sample successor according to probabilities
    std::vector<ValueType> probabilities(actionRow.size());
    std::transform(actionRow.begin(), actionRow.end(), probabilities.begin(), [] (storm::storage::MatrixEntry<exploration_state_type, ValueType> const& entry) { return entry.getValue(); });
    std::discrete_distribution<StateType> distribution(probabilities.begin(), probabilities.end());
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
const std::vector<storm::prism::RewardModel> BlackboxWrapperOnWhitebox<StateType, ValueType>::getRewardModels() {
    return program.getRewardModels();
}

template <typename StateType, typename ValueType>
const std::vector<ValueType> BlackboxWrapperOnWhitebox<StateType, ValueType>::getStateRewards(StateType state) {
    StateType stateIdx = stateMappingOutIn[state];
    return stateRewards[stateIdx];
}

template <typename StateType, typename ValueType>
const std::vector<ValueType> BlackboxWrapperOnWhitebox<StateType, ValueType>::getStateActionRewards(StateType state, StateType action) {
    StateType stateIdx = stateMappingOutIn[state];
    return stateActionRewards[std::make_pair(stateIdx, action)];
}

template <typename StateType, typename ValueType>
std::set<std::string> BlackboxWrapperOnWhitebox<StateType, ValueType>::getStateLabels(StateType state) {
    StateType stateIdx = stateMappingOutIn[state];
    //only update stateLabeling if necessary
    if (stateIdx >= stateLabeling.getNumberOfItems()) {
        stateLabeling = stateGenerationLabels.label();
    }
    return stateLabeling.getLabelsOfState(stateIdx);
}

template <typename StateType, typename ValueType>
std::set<std::string> BlackboxWrapperOnWhitebox<StateType, ValueType>::getActionLabels(StateType state, StateType action) {
    StateType stateIdx = stateMappingOutIn[state];
    return actionLabels[std::make_pair(stateIdx, action)];
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

    // store state rewards
    stateRewards[stateIdx] = behavior.getStateRewards();
    
    StateType startAction = explorationInformation.getActionCount();
    explorationInformation.addActionsToMatrix(behavior.getNumberOfChoices());
    StateType localAction = 0;
    

    if (behavior.getNumberOfChoices() == 0) {
        // make this an artificial sink state
        actionLabels[std::make_pair(stateIdx, 0)] = {"no action defined"};
        for (int i = 0; i < getRewardModels().size(); i++) {
            stateActionRewards[std::make_pair(stateIdx, 0)].emplace_back(0);
        }
        explorationInformation.addActionsToMatrix(1);
        explorationInformation.getRowOfMatrix(startAction).emplace_back(stateIdx, stateIdx);
    } else {
        for (auto const& choice : behavior) {
            if (choice.hasLabels()) {
                actionLabels[std::make_pair(stateIdx, localAction)] = choice.getLabels();
            }
            stateActionRewards[std::make_pair(stateIdx, localAction)] = choice.getRewards();
            for (auto const& entry : choice) {
            explorationInformation.getRowOfMatrix(startAction + localAction).emplace_back(entry.first, entry.second);
            }
            ++localAction;
        }
    }

    explorationInformation.terminateCurrentRowGroup();
    explorationInformation.removeUnexploredState(unexploredIt);
}

template<typename StateType, typename ValueType>
GreyboxWrapperOnWhitebox<StateType, ValueType>::GreyboxWrapperOnWhitebox(storm::prism::Program const& program) : BlackboxWrapperOnWhitebox<StateType, ValueType>(program) {}

template<typename StateType, typename ValueType>
bool GreyboxWrapperOnWhitebox<StateType, ValueType>::isGreybox() {
    return true;
}

template <typename StateType, typename ValueType>
StateType GreyboxWrapperOnWhitebox<StateType, ValueType>::getSucCount(StateType state, StateType action) {
    StateType stateIdx = BlackboxWrapperOnWhitebox<StateType, ValueType>::stateMappingOutIn.at(state);
    StateType stateRowIdx = BlackboxWrapperOnWhitebox<StateType, ValueType>::explorationInformation.getStartRowOfGroup(BlackboxWrapperOnWhitebox<StateType, ValueType>::explorationInformation.getRowGroup(stateIdx));
    auto& actionRow = BlackboxWrapperOnWhitebox<StateType, ValueType>::explorationInformation.getRowOfMatrix(stateRowIdx + action);

    return actionRow.size();
}

template class BlackboxMDP<uint32_t, double>;
template class BlackboxMDP<uint64_t, double>;

template class BlackboxWrapperOnWhitebox<uint32_t, double>;
template class BlackboxWrapperOnWhitebox<uint64_t, double>;

template class GreyboxWrapperOnWhitebox<uint32_t, double>;
template class GreyboxWrapperOnWhitebox<uint64_t, double>;

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
