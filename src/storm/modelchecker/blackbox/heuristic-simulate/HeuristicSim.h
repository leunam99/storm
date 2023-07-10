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
// TODO move seed and random generato to this class
template <typename StateType, typename ValueType>
class HeuristicSim {
    protected:
     typedef StateType ActionType;
     typedef std::vector<std::pair<StateType, ActionType>> StateActionStack;         
     std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp;

    public:
     /*!
      * Constructor for HeuristicSim for a BlackboxMDP
      */
     HeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp);

     /*!
      * returns the type of this simulation heuristic
      * @return HeuristicSimType
      */
     virtual HeuristicSimType getType() = 0;

     /*!
      * returns whether further exploration of the given path should be stopped
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
};


template <typename StateType, typename ValueType>
class NaiveHeuristicSim : public HeuristicSim<StateType, ValueType> {
    using ActionType = typename HeuristicSim<StateType, ValueType>::ActionType; 
    using StateActionStack = typename HeuristicSim<StateType, ValueType>::StateActionStack;   

    public:
     NaiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp, std::seed_seq& seed);
     
     NaiveHeuristicSim(std::shared_ptr<storm::modelchecker::blackbox::BlackboxMDP<StateType>> blackboxMdp);

     /*!
      * returns the type of this simulation heuristic
      * @return HeuristicSimType
      */
     HeuristicSimType getType() override {
        return HeuristicSimType::NAIVE;
     }

     /*!
      * returns whether further exploration of the given path should be stopped
      * @param pathHist current exploration path
      * @return bool
      */
     bool shouldStopSim(StateActionStack& pathHist) override;
 
     /*!
      * sample next action to take on basis of of given exploration path
      * @return Action
      */
     ActionType sampleAction(StateActionStack& pathHist) override;

     /*!
      * reset any memory this heuristic has collected dor its decision making
      */
     void reset() override;

    private:
     mutable std::default_random_engine randomGenerator;

};

} //namespace heuristicSim
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_HEURISTICSIM_H
