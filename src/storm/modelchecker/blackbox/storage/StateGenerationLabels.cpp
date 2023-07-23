#include "storm/builder/BuilderOptions.h"
#include "storm/modelchecker/blackbox/storage/StateGenerationLabels.h"

namespace storm {
namespace modelchecker {
namespace exploration_detail {

template<typename StateType, typename ValueType>
StateGenerationLabels<StateType, ValueType>::StateGenerationLabels(storm::prism::Program const& program, ExplorationInformation<StateType, ValueType>& explorationInformation,
                                                                   storm::expressions::Expression const& conditionStateExpression, storm::expressions::Expression const& targetStateExpression)
    : StateGeneration<StateType, ValueType>::StateGeneration(program, storm::generator::NextStateGeneratorOptions(true, true), explorationInformation, conditionStateExpression, targetStateExpression) {

}


template<typename StateType, typename ValueType>
storm::models::sparse::StateLabeling StateGenerationLabels<StateType, ValueType>::label(std::vector<StateType> const& deadlockStateIndices) {
    return StateGeneration<StateType, ValueType>::generator.label(StateGeneration<StateType, ValueType>::stateStorage, StateGeneration<StateType, ValueType>::getInitialStates(), deadlockStateIndices);
}

template class StateGenerationLabels<uint32_t, double>;

}  // namespace exploration_detail
}  // namespace modelchecker
}  // namespace storm
