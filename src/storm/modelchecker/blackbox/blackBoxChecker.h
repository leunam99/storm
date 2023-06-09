//
// Created by Maximilian Kamps on 25.05.23.
//

/*
kleine Anleitung um die blackbox engine auszuführen
Befehl:
    storm --prism <path-to-mdp-file> --prop <non-trivial-prop> --engine blackbox
    -> simuliert den mdp und speichert die gesampleten Pfade in einem eMDP. Der eMDP wird dann geprinted.
    -> <non-trivial-prop> darf nicht wie "Pmax=? [F true]" sein. Sonst funktioniert der StateGenerator aus irgendeinem Grund nicht richtig.

extra flags: (siehe storm --help blackbox)
    --blackbox:stepssim <count> 
        Sets the number of paths sampled for one simulation step. <count> (default: 30)
    --blackbox:simheuristic <name> 
        Sets the heuristic used for simulation. <name> (in {naive}; default: naive):
        The name of the heuristic to use. 'naive' each time chooses a random action.
    --[blackbox:]precision (eps) <value> The precision to achieve. (To be implemented) <value> (in (0, 1); default: 1e-06):
        The value to use to determine convergence.

*/

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
