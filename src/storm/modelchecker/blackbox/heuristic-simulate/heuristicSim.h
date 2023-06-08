//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_HEURISTICSIM_H
#define STORM_HEURISTICSIM_H

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
     virtual HeuristicsSim getType() = 0;
};

class naiveHeuristicSim : heuristicSim {
    public:
     HeuristicsSim getType() {
        return HeuristicsSim::NAIVE;
     }
};

} //namespace heuristic_simulate
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_HEURISTICSIM_H
