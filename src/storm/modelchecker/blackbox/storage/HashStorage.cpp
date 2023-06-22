#include "HashStorage.h"

namespace storm {
namespace modelchecker {
namespace blackbox {
namespace storage {

//-------------------- Iterator Methods ---------------------------//

template<typename StateType>
KeyIterator<StateType>::KeyIterator(void* map) {
    cur = (*(std::unordered_map<int, void*>*) map).begin();
    end = (*(std::unordered_map<int, void*>*) map).end();
}

template<typename StateType>
KeyIterator<StateType>::KeyIterator() {
    std::unordered_map<int, void*> empty_map;
    cur = empty_map.begin();
    end = empty_map.end();
}

template<typename StateType>
StateType KeyIterator<StateType>::next() {
    StateType key = cur->first;
    cur++;
    return key;
}

template<typename StateType>
bool KeyIterator<StateType>::hasNext() {
    return cur != end;
} 
//--------------------- HashStorage Mathods -------------------------//

template<typename StateType>
HashStorage<StateType>::HashStorage() : data() {
}

template<typename StateType>
std::unordered_map<StateType, StateType> HashStorage<StateType>::get_succ_map(StateType state, StateType action) {
    std::unordered_map<StateType, StateType> succ_map;
    if (data.find(state) == data.end()) {
        return succ_map;
    }

    auto act_map = &data.at(state);
    if (act_map->find(action) == act_map->end()) {
        return succ_map;
    }

    succ_map = act_map->at(action).second;
    return succ_map;
}

template<typename StateType>
void HashStorage<StateType>::add_state(StateType state) {
    if (data.find(state) == data.end())
           data[state] = std::unordered_map<StateType, count_sampleMap_pair>();
}

//__________________ Add states and actions to Datastructure __________//

template<typename StateType>
void HashStorage<StateType>::add_state_actions(StateType state, std::vector<StateType> actions) {
    add_state(state);
    action_count_map[state] = actions.size();
    auto* act_map = &data.at(state);
    if (act_map->begin() == act_map->end()) {
        for (auto act : actions) (*act_map)[act] = count_sampleMap_pair();
    }
}

template<typename StateType>
void HashStorage<StateType>::inc_trans(StateType state, StateType action, StateType succ, StateType samples) {
    add_state(state);  // add state to data if it doesn't exist
    add_state(succ);   // add succ to data if it doesn't exist

    auto* act_map = &data.at(state);  // add action to data if it doesn't exist
    if (act_map->find(action) == act_map->end()) {
        action_count_map[state] += 1;
        (*act_map)[action] = count_sampleMap_pair();
    }

    auto* sample_map = &(act_map->at(action).second);
    (*act_map).at(action).first += samples;  // Increment the total samples for the action
    (*sample_map)[succ] += samples;          // Increments the samples for the (state,action,succ) triple
}

//__________________ Access the Datastructure via Vectors _________________//

template<typename StateType>
std::vector<StateType> HashStorage<StateType>::get_state_vec() {
    std::vector<StateType> state_vec;
    for (auto const& p : data) state_vec.push_back(p.first);
    return state_vec;
}

template<typename StateType>
std::vector<StateType> HashStorage<StateType>::get_state_actions_vec(StateType state) {
    std::vector<StateType> action_vec;
    if (data.find(state) != data.end())
        for (auto const& p : data.at(state)) action_vec.push_back(p.first);
    return action_vec;
}

template<typename StateType>
std::vector<StateType> HashStorage<StateType>::get_state_action_succ_vec(StateType state, StateType action) {
    std::vector<StateType> succ_vec;
    for (auto const& p : get_succ_map(state, action)) succ_vec.push_back(p.first);
    return succ_vec;
}
//__________________ Access the Datastructure via Iterators _________________//

template<typename StateType>
KeyIterator<StateType> HashStorage<StateType>::get_state_itr() {
    return KeyIterator<StateType>(&data);
}

template<typename StateType>
KeyIterator<StateType> HashStorage<StateType>::get_state_actions_itr(StateType state) {
    if (data.find(state) == data.end()) {
        return KeyIterator<StateType>();
    }
    return KeyIterator<StateType>(&data.at(state));
}

template<typename StateType>
KeyIterator<StateType> HashStorage<StateType>::get_state_action_succ_itr(StateType state, StateType action) {
    if (data.find(state) == data.end()) {
        return KeyIterator<StateType>();
    }
    auto* act_map = &data.at(state);
    if(act_map->find(action) == act_map->end()) {
        return KeyIterator<StateType>();
    }
    return KeyIterator<StateType>(&(act_map->at(action).second));
}

//__________________ Access seperate elements of the Datastructure _____________________//

template<typename StateType>
bool HashStorage<StateType>::state_exists(StateType state) {
    return data.find(state) != data.end();
}

template<typename StateType> 
StateType HashStorage<StateType>::get_action_count(StateType state) {
    if(action_count_map.find(state) == action_count_map.end()) 
        return -1;
    return action_count_map[state];
}

template<typename StateType>
StateType HashStorage<StateType>::get_total_samples(StateType state, StateType action) {
    if (data.find(state) == data.end()) {
        return -1;
    }

    auto* act_map = &data.at(state);
    if (act_map->find(action) == act_map->end()) {
        return -1;
    }
    return act_map->at(action).first;
}

template<typename StateType>
StateType HashStorage<StateType>::get_succ_samples(StateType state, StateType action, StateType succ) {
    auto succ_map = get_succ_map(state, action);

    if (succ_map.find(succ) != succ_map.end())
        return succ_map.at(succ);
    return -1;
}

template<typename StateType>
void HashStorage<StateType>::set_succ_count(std::pair<StateType, StateType> state_action_pair, StateType count) {
    succ_count_map[state_action_pair] = count;
}

template<typename StateType>
StateType HashStorage<StateType>::get_succ_count(std::pair<StateType, StateType> state_action_pair) {
    if(succ_count_map.find(state_action_pair) != succ_count_map.end())
        return succ_count_map[state_action_pair];
    return -1;
}

template<typename StateType>
void HashStorage<StateType>::print() {
    for (auto state : get_state_vec()) {
        std::cout << "-------------------------\n";
        std::cout << "State: " << state << "\n";
        for (auto action : get_state_actions_vec(state)) {
            std::cout << " Action: " << action << " | Total Samples: " << get_total_samples(state, action) << "\n";
            for (auto succ : get_state_action_succ_vec(state, action)) {
                std::cout << "  * Succ: " << succ << " | Samples: " << get_succ_samples(state, action, succ) << "\n";
            }
        }
    }
}
template class HashStorage<int_fast32_t>; //Type for which class gets compiled 
template class HashStorage<int_fast64_t>; //Type for which class gets compiled 
template class KeyIterator<int_fast32_t>; //Type for which class gets compiled 
template class KeyIterator<int_fast64_t>; //Type for which class gets compiled 
}
}
}
}


int main(int argc, char const *argv[])
{       
    
    auto x = storm::modelchecker::blackbox::storage::HashStorage<int_fast32_t>();
    x.inc_trans(1,2,3,10);
    x.inc_trans(1,2,7,5);
    x.inc_trans(1,2,4,3);
    x.inc_trans(1,3,5,3);

    x.set_succ_count(std::make_pair(1,2), 4);
    std::cout << x.get_succ_count(std::make_pair(1,2)) << " meh\n";
    std::cout << x.get_succ_count(std::make_pair(1,3)) << " meh\n";
    std::cout << x.get_action_count(1) << " www \n";

    //auto i1 = x.get_state_itr();

    auto i1 = x.get_state_itr();

    while (i1.hasNext())
    {
        std::cout << i1.next() << "\n";
    }
    std::cout << "next itr \n";
    
    auto i2 = x.get_state_actions_itr(1);

    while (i2.hasNext())
    {
        std::cout << i2.next() << "\n";
    }

    auto i3 = x.get_state_action_succ_itr(1,2);
    std::cout << "next itr \n";
    while (i3.hasNext())
    {
        std::cout << i3.next() << "\n";
    }
}




