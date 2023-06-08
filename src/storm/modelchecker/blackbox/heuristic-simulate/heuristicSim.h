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

class heuristicSim {
    virtual HeuristicsSim getType() = 0;
};

class naiveHeuristicSim : heuristicSim {
    HeuristicsSim getType() {
       return HeuristicsSim::NAIVE;
    }
};

} //namespace heuristic_simulate
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_HEURISTICSIM_H
