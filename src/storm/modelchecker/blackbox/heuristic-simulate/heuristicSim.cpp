//
// Created by Maximilian Kamps on 25.05.23.
//

#include "heuristicSim.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace heuristic_simulate{

std::unique_ptr<heuristicSim> getHeuristicSimFromType(HeuristicsSim heuristic) {
    switch (heuristic) {
        case HeuristicsSim::NAIVE:
            return std::make_unique<heuristicSim>(naiveHeuristicSim());
        default:
            STORM_LOG_THROW(true, storm::exceptions::InvalidTypeException, "Simulation heuristic " << heuristic << "is not implemented. Use naive.");
            return std::make_unique<heuristicSim>(naiveHeuristicSim());
    }
    
}

} //namespace heuristic_simulate
} //namespace blackbox
} //namespace modelchecker
} //namespace storm