#pragma once

#include "storm/modelchecker/AbstractModelChecker.h"
#include "storm/modelchecker/blackbox/BlackboxInterface.h"

#include "storm/storage/prism/Program.h"

/*
This Checker implements the SMC-Algorithm for blackbox MDP from paper 
"Statistical Model Checking Blackbox MDP via Families of Bounded Parameter MDP" by Maximilian Weiniger <weininge@in.tum.de>

The code for the SIMULATE phase can be found in files 
    - Simulator 
    - heuristicSim/HeuristicSim

The code for the INFER phase can be found in files
    - infer

The code for the data structures eMDP and bMDP can be found in the files with the respective name

The oracle for a blackbox MDP is defined in BlackboxInterface
*/
namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ModelType, typename StateType>
class BlackboxChecker: public storm::modelchecker::AbstractModelChecker<ModelType> {
    public:
     typedef typename ModelType::ValueType ValueType;

     BlackboxChecker(storm::prism::Program const& program);

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
     std::shared_ptr<BlackboxMDP<StateType, ValueType>> blackboxMDP;

};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
