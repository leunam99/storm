#pragma once

#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/storage/expressions/ExpressionEvaluator.h"
#include "storm/models/sparse/StateLabeling.h"


namespace storm {
namespace expressions {
class Expression;
}

namespace prism {
class Program;    
}

namespace modelchecker {
namespace exploration_detail {

template<typename StateType, typename ValueType>
class ExplorationInformation;

/*
 * This class simply extends the StateGeneration class with the labels function
 * that returns a StateLabeling for all (discovered) states
 */
template<typename StateType, typename ValueType>
class StateGenerationLabels: public StateGeneration<StateType, ValueType>{
   public:
    StateGenerationLabels(storm::prism::Program const& program, ExplorationInformation<StateType, ValueType>& explorationInformation,
                          storm::expressions::Expression const& conditionStateExpression, storm::expressions::Expression const& targetStateExpression);
     
    storm::models::sparse::StateLabeling label(std::vector<StateType> const& deadlockStateIndices = {});
};

}  // namespace exploration_detail
}  // namespace modelchecker
}  // namespace storm