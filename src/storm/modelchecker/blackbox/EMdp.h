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
class EMdp {
   public:
    /*!
     * Constructs an empty EMdp
     */
    EMdp();

    /*!
     * adds initial state to EMdp
     * 
     * @param state initial state 
     */
    void addInitialState(StateType state);

    /*!
     * retrieves initial state
     * 
     * @param state initial state 
     */
    StateType getInitialState();

    /*!
     * Add a new state to the EMdp
     * @param state  state index of new state
     * @param availActions vector of available actions at state
     */
    void addState(StateType state, std::vector<StateType> availActions);

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

    void addVisits(StateType state, StateType action, StateType succ, uint64_t visits);

    /*!
     * Add action that is reachable from the state to the emdp, without having to add a successor state 
     * (because the successor state might not have been sampled yet)
     * 
     * @param state 
     * @param action 
     */
    void addUnsampledAction(StateType state, StateType action);
    
    
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
     * Converts a vector of state or transition labels to string 
     * 
     * @param labelVec 
     * @return std::string 
     */
    std::string labelVecToStr(const std::vector<std::string>& labelVec);

    /*!
    * Writes the Emdp to a file 
    *
    * @param fileName
    */
    void eMdpToFile(const std::string& fileName);

    /*!
     * Constructs an EMdp from a File 
     * 
     * @param fileName the File containing the Emdp 
     * @return EMdp<StateType> 
     */
    static EMdp<StateType> eMdpFromFile(const std::string& fileName);

    /*!
     * print the EMdp to std::cout
     */
    void print();

    /*!
     * returns true. if the state was already added to this EMdp. false otherwise
     * @param state  state index of tested state
     */
    bool isStateKnown(StateType state);
    
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
     * returns how often this state action pair was sampled
     * @param state   state index 
     * @param action  action index
     */
    uint64_t getSampleCount(StateType state, StateType action);

    /*!
     * returns how often this state action successor triple was sampled
     * @param state   state index
     * @param action  action index
     * @param succ    successor state index
     */
    uint64_t getSampleCount(StateType state, StateType action, StateType succ);

    /*!
     * Set the number of successors for a state action pair in the greybox setting 
     * 
     * @param state 
     * @return StateType 
     */
    void setSuccCount(StateType state, StateType action, int count);

    /*!
     * Get the number of successors for a state action pair in the greybox setting 
     * 
     * @return StateType 
     */
    int getSuccCount(StateType state, StateType action);

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
    
   storage::HashStorage<StateType> hashStorage;
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
   StateType initState;
   bool initStateValid = false;
   StateType explorationCount = 0; //Number of explored states
};

} //namespace blackbox
} //namespace modelchecker
} //namespace storm
