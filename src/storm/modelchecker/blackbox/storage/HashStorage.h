#include <unordered_map>
#include <stdint.h>
#include <vector>
#include <utility>
#include <iostream>



namespace storm {
namespace modelchecker {
namespace blackbox {
namespace storage {


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

    using count_sampleMap_pair = std::pair<StateType, std::unordered_map<StateType, StateType> >;
    std::unordered_map<StateType, std::unordered_map<StateType, count_sampleMap_pair > > data;


    //TODO: Use boost::hash instead, keep pair_hash for easy compile
    struct pair_hash {
        template <class T1, class T2>
        std::size_t operator () (const std::pair<T1,T2> &p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);

            return h1 ^ h2;  
        }
    };

    // Maps state action pair to number of known successors -> used in greybox setting 
    std::unordered_map<std::pair<StateType, StateType>, StateType, pair_hash> succ_count_map;
    
    // Maps state to the number of available actions
    std::unordered_map<StateType, StateType> action_count_map;

    // Maps states to their predecessors action pair (created on demand for debugging)
    std::unordered_map<StateType, std::vector<std::pair<StateType, StateType> > > reverse_map;

    /*!
     * Helper function, returns the succ_map of a (state,action) Pair
     * 
     * @param state 
     * @param action 
     * @return std::unordered_map<StateType, StateType>
     */
    std::unordered_map<StateType, StateType> get_succ_map(StateType state, StateType action);

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
    void add_state(StateType state);

    /*!
     * Adds a vector of actions to the state 
     * (only if the state does not have any actions yet!)
     * 
     * @param state 
     * @param actions
     */
    void add_state_actions(StateType state, std::vector<StateType> actions);

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
    void inc_trans(StateType state, StateType action, StateType succ, StateType samples);

    /*!
     * Returns a vector of all states 
     * 
     * @return std::vector<StateType>
     */
    std::vector<StateType> get_state_vec();

    /*!
     * Returns a vector of available actions for the state  
     * 
     * @param state 
     * @return std::vector<StateType>
     */
    std::vector<StateType> get_state_actions_vec(StateType state);

    /*!
     * Return as vector of successors for a state action pair 
     * 
     * @param state 
     * @param action 
     * @return std::vector<StateType> 
     */

    std::vector<StateType> get_state_action_succ_vec(StateType state, StateType action);

    /*!
     * Returns a KeyIterator over the states 
     * 
     * @return KeyIterator<StateType> 
     */
    KeyIterator<StateType> get_state_itr();

    /*!
     * Returns a KeyIterator over the actions of a state
     * 
     * @param state 
     * @return KeyIterator<StateType> 
     */
    KeyIterator<StateType> get_state_actions_itr(StateType state);

    /*!
     * Returns a KeyIterator over the successor of a state for a given action 
     * 
     * @param state 
     * @param action 
     * @return KeyIterator<StateType> 
     */
    KeyIterator<StateType> get_state_action_succ_itr(StateType state, StateType action);


    /**
     * Get the (state,action) predecessor vector of state
     * 
     * @param state 
     * @return std::vector<std::pair<StateType, StateType> > 
     */
    std::vector<std::pair<StateType, StateType> > get_predecessors(StateType state);

    /*!
     * Returns true if the passed state is in data 
     * 
     * @param state 
     * @return true 
     * @return false 
     */
    bool state_exists(StateType state);

    /*!
     * Returns the number of actions for a state 
     * 
     * @param state 
     * @return StateType 
     */
    StateType get_action_count(StateType state);

    /*!
     * Returns the total samples for a given state and action 
     * 
     * @param state 
     * @param action 
     * @return StateType
     */
    StateType get_total_samples(StateType state, StateType action);

    /*!
     * Returns the samples for a (state,action,succ) triple
     * 
     * @param state 
     * @param action 
     * @param succ 
     * @return StateType
     */
    StateType get_succ_samples(StateType state, StateType action, StateType succ);

    /*!
     * Set the number of successors for a (state,action) pair 
     * (used in greybox case)
     * 
     * @param state_action_pair 
     * @param count 
     */
    void set_succ_count(std::pair<StateType, StateType> state_action_pair, StateType count);

    /*!
     * get the number of successors for a (state,action) pair 
     * (used in greybox case)
     * 
     * @param state_action_pair 
     * @param count 
     */
    StateType get_succ_count(std::pair<StateType, StateType> state_action_pair);

    /*!
     * Creates a mapping to the (state,action) predecessors of every state. 
     * Is generated on demand
     * Used for the visualization of neighborhoods in eMdpToDot
     */
    void create_reverse_mapping();

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
