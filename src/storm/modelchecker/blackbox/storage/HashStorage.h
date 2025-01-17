#pragma once

#include <unordered_map>
#include <stdint.h>
#include <vector>
#include <utility>
#include <iostream>
#include <boost/functional/hash.hpp>



namespace storm {
namespace modelchecker {
namespace blackbox {
namespace storage {

/**
 * Used to iterate over the keys the consequitive hashStorage maps
 * (Is independent of the value unordered_map value type)   
 */
template<typename StateType> 
class KeyIterator {
    private: 
     std::unordered_map<int, void*>::iterator cur;
     std::unordered_map<int, void*>::iterator end;
    public:
     KeyIterator(void* map);
     KeyIterator();
     
     StateType peek();
     StateType next();
     bool hasNext();
}; 

/*!
 * Class to store eMDPs
 * The underlying data structure uses consequitive hashmaps and NOT matrices
 */
template<typename StateType>
class HashStorage {
   private:
    /*!
    * eMDPs are saved as 3 consequitive hashmaps 
    * 1.) key = state      | value = 2.) hashmap 
    * 2.) key = action     | value = pair (#total samples, 3.) hashmap)
    * 3.) key = succ       | value = #samples of the (state,action,succ) triple
    */

    using countSampleMapPair = std::pair<StateType, std::unordered_map<StateType, uint64_t> >;
    std::unordered_map<StateType, std::unordered_map<StateType, countSampleMapPair > > data;

    // Maps state action pair to number of known successors -> used in greybox setting 
    typedef std::pair<StateType, StateType> stateTypePair;
    std::unordered_map<std::pair<StateType, StateType>, int, boost::hash<stateTypePair> > succCountMap;

    // Maps states to their predecessors action pair (created on demand for debugging)
    std::unordered_map<StateType, std::vector<stateTypePair > > reverseMap;

    StateType totalStateCount = 0;
    StateType totalStateActionPairCount = 0;
    StateType totalTransitionCount = 0;

    /*!
     * Helper function, returns the succMap of a (state,action) Pair
     * 
     * @param state 
     * @param action 
     * @return std::unordered_map<StateType, StateType>
     */
    std::unordered_map<StateType, uint64_t> getSuccMap(StateType state, StateType action);

   public:
    /**
     * Construct empty Hash Storage 
     * 
     */
    HashStorage();

    /*!
     * adds state do data 
     * sets the #actions available to the state to -1
     * 
     * @param state
     */
    void addState(StateType state);

    /*!
     * Adds a vector of actions to the state 
     * (only if the state does not have any actions yet!)
     * 
     * @param state 
     * @param actions
     */
    void addStateActions(StateType state, std::vector<StateType> actions);

    /**
     * Add action that is reachable from the state to HashStorage, without having to add a successor state 
     * (because the successor state might not have been sampled yet)
     * 
     * @param state 
     * @param action 
     */
    void addUnsampledAction(StateType state, StateType action);

    /*!
     * Increments a transition of the form (state,action,succ) = samples
     * If state or succ don't exists yet they get added to data 
     * Increments the total samples of the action by samples 
     * 
     * @param state 
     * @param action 
     * @param succ 
     * @param samples
     */
    void incTrans(StateType state, StateType action, StateType succ, uint64_t samples);

    /*!
     * Returns a vector of all states 
     * 
     * @return std::vector<StateType>
     */
    std::vector<StateType> getStateVec();

    /*!
     * Returns a vector of available actions for the state  
     * 
     * @param state 
     * @return std::vector<StateType>
     */
    std::vector<StateType> getStateActionVec(StateType state);

    /*!
     * Return as vector of successors for a state action pair 
     * 
     * @param state 
     * @param action 
     * @return std::vector<StateType> 
     */

    std::vector<StateType> getStateActionSuccVec(StateType state, StateType action);

    /*!
     * Returns a KeyIterator over the states 
     * 
     * @return KeyIterator<StateType> 
     */
    KeyIterator<StateType> getStateItr();

    /*!
     * Returns a KeyIterator over the actions of a state
     * 
     * @param state 
     * @return KeyIterator<StateType> 
     */
    KeyIterator<StateType> getStateActionsItr(StateType state);

    /*!
     * Returns a KeyIterator over the successor of a state for a given action 
     * 
     * @param state 
     * @param action 
     * @return KeyIterator<StateType> 
     */
    KeyIterator<StateType> getStateActionsSuccItr(StateType state, StateType action);


    /**
     * Get the (state,action) predecessor vector of state
     * 
     * @param state 
     * @return std::vector<std::pair<StateType, StateType> > 
     */
    std::vector<std::pair<StateType, StateType> > getPredecessors(StateType state);

    /*!
     * Returns true if the passed state is in data 
     * 
     * @param state 
     * @return true 
     * @return false 
     */
    bool stateExists(StateType state);

    /*!
     * Return the total number of States 
     * 
     * @return StateType 
     */
    StateType getTotalStateCount();

    /*!
     * Return the total number of state action pairs 
     * 
     * @return StateType 
     */
    StateType gettotalStateActionPairCount();

    /**
     * Return the total number of transitions 
     * 
     * @return StateType 
     */
    StateType getTotalTransitionCount();

    /*!
     * Returns the total samples for a given state and action 
     * 
     * @param state 
     * @param action 
     * @return StateType
     */
    uint64_t getTotalSamples(StateType state, StateType action);

    /*!
     * Returns the samples for a (state,action,succ) triple
     * 
     * @param state 
     * @param action 
     * @param succ 
     * @return uint64_t
     */
    uint64_t getSuccSamples(StateType state, StateType action, StateType succ);

    /*!
     * Set the number of successors for a (state,action) pair 
     * (used in greybox case)
     * 
     * @param stateActionPair 
     * @param count 
     */
    void setSuccCount(stateTypePair stateActionPair, int count);

    /*!
     * get the number of successors for a (state,action) pair 
     * (used in greybox case)
     * 
     * @param stateActionPair 
     * @param count 
     */
    int getSuccCount(stateTypePair stateActionPair);

    /*!
     * Creates a mapping to the (state,action) predecessors of every state. 
     * Is generated on demand
     * Used for the visualization of neighborhoods in eMdpToDot
     */
    void createReverseMapping();

    /*!
     * Prints the data structure to std::cout
     *
     */
    void print();
};
}
}
}
}
