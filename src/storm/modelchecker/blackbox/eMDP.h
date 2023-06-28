#ifndef STORM_EMDP_H
#define STORM_EMDP_H

#pragma once
#include <string.h>
#include "stdint.h"
#include <iostream>
#include <fstream>

#include "storage/HashStorage.h"


namespace storm {
namespace modelchecker {
namespace blackbox {


template<typename StateType>

class eMDP {
   public:
    /*!
     * Constructs an empty eMDP
     */
    eMDP();

    /*!
     * adds initial state to eMDP
     * 
     * @param state initial state 
     */
    void addInitialState(StateType state);
    
    /*!
     * increments the visits count of the given triple by 1
     * @param state   state index in which action was taken 
     * @param action  action index of chosen action
     * @param succ    state index of successor state
     */
    void addVisit(StateType state, StateType action, StateType succ);

    /*!
     * increments the visits count of the given by visits
     * @param state   state index in which action was taken 
     * @param action  action index of chosen action
     * @param succ    state index of successor state
     * @param visits  visits to be added to counter
     */
    void addVisits(StateType state, StateType action, StateType succ, StateType visits);

    /*!
     * Add a new state to the eMDP
     * @param state  state index of new state
     * @param availActions vector of available actions at state
     */
    void addState(StateType state, std::vector<StateType> availActions);
    
    /*!
     * Adds a new Label to the state 
     * 
     * @param label 
     * @param state 
     */
    void addStateLabel(std::string label, StateType state);
    
    /*!
     * Removes a label from the state 
     * 
     * @param label 
     * @param state 
     */
    void removeStateLabel(std::string label, StateType state);
    
    /*!
     * Returns the vector of Labels for a state 
     * 
     * @param state 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> getStateLabels(StateType state);

    /*!
     * Adds a new Label to the Action 
     * 
     * @param label 
     * @param state 
     */
    void addActionLabel(std::string label, StateType state, StateType action);
    
    /*!
     * Removes a label from the Action
     * 
     * @param label 
     * @param state 
     */
    void removeActionLabel(std::string label, StateType state, StateType action);

    /*!
     * Returns the vector of Labels for an action
     * 
     * @param state 
     * @return std::vector<std::string> 
     */
    std::vector<std::string> getActionLabels(StateType state, StateType action);

    /*!
     * print the eMDP to std::cout
     */
    void print();

    /*!
     * returns true. if the state was already added to this eMDP. false otherwise
     * @param state  state index of tested state
     */
    bool isStateKnown(StateType state);

    /*!
     * returns how often this state action pair was sampled
     * @param state   state index 
     * @param action  action index
     */
    StateType getSampleCount(StateType state, StateType action);

    /*!
     * returns how often this state action successor triple was sampled
     * @param state   state index
     * @param action  action index
     * @param succ    successor state index
     */
    StateType getSampleCount(StateType state, StateType action, StateType succ);

    /*!
     * Set the number of successors for a state action pair in the greybox setting 
     * 
     * @param state 
     * @return StateType 
     */
    void setSuccCount(StateType state, StateType action, StateType count);

    /*!
     * Get the number of successors for a state action pair in the greybox setting 
     * 
     * @return StateType 
     */
    StateType getSuccCount(StateType state, StateType action);

    /**
     * Get the number of actions for a state 
     * 
     * @param state 
     * @return StateType 
     */
    StateType getActionCount(StateType state); 

    //? Save to disk

    /*!
     * Returns a KeyIterator over the states 
     * 
     * @return KeyIterator<StateType> 
     */
    storage::KeyIterator<StateType> getStateItr();

    /*!
     * Returns a KeyIterator over the actions of a state
     * 
     * @param state 
     * @return KeyIterator<StateType> 
     */
    storage::KeyIterator<StateType> getStateActionsItr(StateType state);

    /*!
     * Returns a KeyIterator over the successor of a state for a given action 
     * 
     * @param state 
     * @param action 
     * @return KeyIterator<StateType> 
     */
    storage::KeyIterator<StateType> getStateActionsSuccItr(StateType state, StateType action);
    
    void createReverseMapping();

    /**
     * @brief Get the (state,action) predecessor vector of state
     * 
     * @param state 
     * @return std::vector<std::pair<StateType, StateType> > 
     */
    std::vector<std::pair<StateType, StateType> > getPredecessors(StateType state);

   private:
    /*!
     * Adds the state and its corresponding exploration time to explorationOrder
     *
     * @param state
     */
    void addStateToExplorationOrder(StateType state);

   storage::HashStorage<StateType> hashStorage;
   std::unordered_map<StateType, StateType> explorationOrder; // maps state to its position of when its been found
   std::unordered_map<StateType, std::vector<std::string> > stateLabeling; 

   //TODO: Use boost::hash instead, keep pairHash for easy compile
   
    struct pairHash {
        template <class T1, class T2>
        std::size_t operator () (const std::pair<T1,T2> &p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);

            return h1 ^ h2;  
        }
    };
    

   std::unordered_map<std::pair<StateType, StateType>, std::vector<std::string>, pairHash > actionLabeling; 
   StateType initState = -1;
   StateType explorationCount = 0; //Number of explored states
};
} //namespace blackbox
} //namespace modelchecker
} //namespace storm
#endif  // STORM_EMDP_H
