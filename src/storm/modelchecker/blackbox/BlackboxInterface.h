#pragma once

/*
 * This header defines the general interface expected from a blackbox MDP
 * 
*/
#include <random>

#include "storm/modelchecker/exploration/StateGeneration.h"
#include "storm/modelchecker/exploration/ExplorationInformation.h"
#include "storm/logic/Formula.h"

namespace storm {
namespace modelchecker {
namespace blackbox {

// TODO add reward system
template <typename StateType>
class BlackboxMDP {
    public:

     /*!
      * returns the state indentifier of the initial state
     */
     virtual StateType getInitialState() = 0;

     /*!
      * returns a KeyIterator over the available actions of the given state 
      * 
      * @param state 
      * @return KeyIterator<index_type> 
      */
     virtual StateType getAvailActions(StateType state) = 0;
     

     /*!
      * sample a random successor from the action on the given state and return the successors state identifier.
      *
      * it is guaranteed that at any time, the indice of all ever returned successors are consecutive. Every new 
      * previously unseen successor will have the next largest index
      * 
      * @param state 
      * @param action
      * @return successor state identfier
      */
     virtual StateType sampleSucc(StateType state, StateType action) = 0;

     /*!
      * returns a lower bound for all transition probilities in this MDP, taken from cli input
      */
     virtual double getPmin();

     /*!
      * returns true if this MDP is a greybox MDP, false if it is a blackbox MDP 
      */
     virtual bool isGreybox() = 0;

     // virtual auto getRewardModels() = 0;

     // virtual auto getStateRewards(StateType state) = 0;

     // virtual auto getStateLabels(StateType state) = 0;

     // virtual auto getActionReward(StateType state, StateType action) = 0;

     // virtual auto getActionLabels(StateType state, StateType action) = 0;
     
     /*!
      * greybox method
      * returns how many successors a state has for a given action 
      * 
      * @param state 
      * @param action
      * 
      * @throws NotSupportedException, NotImplementedException 
      */
     virtual StateType getSucCount(StateType state, StateType action);

     virtual ~BlackboxMDP();
};

template <typename StateType, typename ValueType>
class BlackboxWrapperOnWhitebox: public BlackboxMDP<StateType> {
    typedef uint32_t exploration_state_type; // exploration code only uses uint32_t and is not flexible

    public:
     explicit BlackboxWrapperOnWhitebox(storm::prism::Program const& program);
    
     /*!
      * returns the state indentifier of the initial state
     */
     StateType getInitialState() override;

     /*!
      * returns a KeyIterator over the available actions of the given state 
      * 
      * @param state 
      * @return index_type number of available actions; actions are labeled in ascending order from 0
      */
     StateType getAvailActions(StateType state) override;
     

     /*!
      * sample a random successor from the action on the given state and return the successors state identifier.
      *
      * it is guaranteed that at any time, the indice of all ever returned successors are consecutive. Every new 
      * previously unseen successor will have the next largest index
      * 
      * @param state 
      * @param action
      * @return successor state identfier
      */
     StateType sampleSucc(StateType state, StateType action) override;

     /*!
      * returns true if this MDP is a greybox MDP, false if it is a blackbox MDP 
      */
     bool isGreybox() override;

    protected:
     void exploreState(StateType state);

     storm::prism::Program program;
     storm::modelchecker::exploration_detail::ExplorationInformation<exploration_state_type, ValueType> explorationInformation;
     storm::modelchecker::exploration_detail::StateGeneration<exploration_state_type, ValueType> stateGeneration;
     mutable std::default_random_engine randomGenerator;
     std::unordered_map<StateType, StateType> stateMappingInOut;  // maps internal indice to external
     std::unordered_map<StateType, StateType> stateMappingOutIn;  // maps external indice to internal

};

template <typename StateType, typename ValueType>
class GreyboxWrapperOnWhitebox: public BlackboxWrapperOnWhitebox<StateType, ValueType> {
   public:
    explicit GreyboxWrapperOnWhitebox(storm::prism::Program const& program);
    bool isGreybox() override;
    StateType getSucCount(StateType state, StateType action) override;

};


} //namespace blackbox
} //namespace modelchecker
} //namespace storm