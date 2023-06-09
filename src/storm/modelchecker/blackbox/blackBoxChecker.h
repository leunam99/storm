//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_BLACKBOXCHECKER_H
#define STORM_BLACKBOXCHECKER_H

#include "storm/modelchecker/AbstractModelChecker.h"
#include "storm/modelchecker/blackbox/blackBoxExplorer.h"
#include "storm/modelchecker/blackbox/heuristic-simulate/heuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

template<typename ModelType, typename StateType = uint32_t>
class blackBoxChecker: public storm::modelchecker::AbstractModelChecker<ModelType> {
    public:
     typedef typename ModelType::ValueType ValueType;

     blackBoxChecker(storm::prism::Program const& program);
     
     virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;

     virtual std::unique_ptr<CheckResult> computeUntilProbabilities(Environment const& env,
                                                                    CheckTask<storm::logic::UntilFormula, ValueType> const& checkTask) override;

    private:
     // The program that defines the model to check.
     storm::prism::Program program;
     blackBoxExplorer<StateType, ValueType> blackBoxExpl;
     uint64_t pathsSampledPerSimulation;
     std::unique_ptr<storm::modelchecker::blackbox::heuristic_simulate::heuristicSim> simHeuristic;
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_BLACKBOXCHECKER_H
