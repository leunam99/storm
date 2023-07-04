//
// Created by Maximilian Kamps on 25.05.23.
//

#ifndef STORM_HEURISTICSIM_H
#define STORM_HEURISTICSIM_H

#include <random>

#include "storm/modelchecker/blackbox/BlackboxInterface.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace heuristicSim {

enum HeuristicSimType{NAIVE};

// TODO this heuristic class should be able to return the action to take
//      given the latest path, state and 'other' information.
//      because 'other' is not known right now it has to be implemented later on
template <typename StateType, typename ValueType>
class HeuristicSim {
    typedef StateType ActionType;
    typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;

    public:
     /*!
      * Constructor for HeuristicSim for a BlackboxMDP
      */
     HeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp);

     /*!
      * returns the Type of this simulation heuristic
      * @return HeuristicSimType
      */
     virtual HeuristicSimType getType() = 0;

     /*!
      * returns returns of the exploration of the given path should be stopped
      * @param pathHist current exploration path
      * @return bool
      */
     virtual bool shouldStopSim(StateActionStack& pathHist) = 0;
 
     /*!
      * sample next action to take on basis of of given exploration path
      * @return Action
      */
     virtual ActionType sampleAction(StateActionStack& pathHist) = 0;

      /*!
      * reset any memory this heuristic has collected dor its decision making
      */
     virtual void reset() = 0;

    protected:
     std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp;
};


template <typename StateType, typename ValueType>
class NaiveHeuristicSim : public HeuristicSim<StateType, ValueType> {
    using ActionType = typename HeuristicSim<StateType, ValueType>::ActionType;
    using StateActionStack = typename HeuristicSim<StateType, ValueType>::StateActionStack;

    public:
     NaiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp, std::seed_seq seed);
     NaiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp);

     /*!
      * returns the Type of this simulation heuristic
      * @return HeuristicSimType
      */
     HeuristicSimType getType() {
        return HeuristicSimType::NAIVE;
     }

     /*!
      * returns returns of the exploration of the given path should be stopped
      * @param pathHist current exploration path
      * @return bool
      */
     bool shouldStopSim(StateActionStack& pathHist);
 
     /*!
      * sample next action to take on basis of of given exploration path
      * @return Action
      */
     ActionType sampleAction(StateActionStack& pathHist);

     /*!
      * reset any memory this heuristic has collected dor its decision making
      */
     void reset();

    private:
     mutable std::default_random_engine randomGenerator;

};

} //namespace heuristicSim
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_HEURISTICSIM_H
