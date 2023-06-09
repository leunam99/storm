//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_HEURISTICSIM_H
#define STORM_HEURISTICSIM_H

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidTypeException.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace heuristic_simulate{

enum HeuristicsSim{NAIVE};

// TODO this heuristic class should be able to return the action to take
//      given the latest path, state and 'other' information.
//      because 'other' is not known right now it has to be implemented later on
class heuristicSim {
    public:
     virtual HeuristicsSim getType() {
        // return default value
        return HeuristicsSim::NAIVE;
     }
};

class naiveHeuristicSim : public heuristicSim {
    public:
     HeuristicsSim getType() {
        return HeuristicsSim::NAIVE;
     }
};

std::unique_ptr<heuristicSim> getHeuristicSimFromType(HeuristicsSim heuristic);

} //namespace heuristic_simulate
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_HEURISTICSIM_H
