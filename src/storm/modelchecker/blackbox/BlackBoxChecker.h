//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_BLACKBOXCHECKER_H
#define STORM_BLACKBOXCHECKER_H

#include "storm/modelchecker/AbstractModelChecker.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"

#include "storm/storage/prism/Program.h"


namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ModelType, typename StateType>
class BlackBoxChecker: public storm::modelchecker::AbstractModelChecker<ModelType> {
    public:
     typedef typename ModelType::ValueType ValueType;

     BlackBoxChecker(storm::prism::Program const& program);

     /*!
      * Determines whether the model checker can handle the given verification task. If this method returns
      * false, the task must not be checked using this model checker.
      *
      * @param checkTask The task for which to check whether the model checker can handle it.
      * @return True iff the model checker can check the given task.
      */
     bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
     
     std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env, CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;

    private:
     std::shared_ptr<BlackboxMDP<StateType>> blackboxMDP;

};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXCHECKER_H
